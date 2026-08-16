#include "RulesBridge.h"
#include "Misc/DefaultValueHelper.h"
#include "Misc/DateTime.h"
#include "Misc/Base64.h"

// Forward declaration of native verification binding (implemented in this module as a stub; replace with real crypto lib)
extern "C" bool NativeVerifyEd25519(const uint8* pubkey, int pubkey_len, const uint8* sig, int sig_len, const uint8* msg, int msg_len);

FRollResult UMadCrapsRulesBridge::ExecuteRoll(int32 Seed, const FString& BetConfig, FString& OutSignedToken)
{
    FRollResult Result;

    // TODO: Replace with actual call into the native rules engine.
    // If you're linking a native library, this is where you'd call the C API.
    // For now we provide a deterministic placeholder for testing.

    int32 s = Seed;
    if (s == 0)
    {
        s = (int32)FDateTime::UtcNow().ToUnixTimestamp();
    }

    // Simple pseudo-roll for prototype
    Result.DiceA = (s % 6) + 1;
    Result.DiceB = ((s / 7) % 6) + 1;
    Result.Outcome = Result.DiceA + Result.DiceB;
    Result.DebugInfo = FString::Printf(TEXT("Prototype roll with seed=%d"), s);

    // TODO: Call server signing routine and set OutSignedToken to the signed blob
    OutSignedToken = FString::Printf(TEXT("PROTOTYPE_TOKEN_%d"), s);

    return Result;
}

bool UMadCrapsRulesBridge::VerifySignedRoll(const FString& SignedToken, FRollResult& OutResult)
{
    // TODO: Implement signature verification logic (e.g., Ed25519 or ECDSA) using bundled public key.
    // For now, accept prototype tokens and parse seed.
    if (SignedToken.StartsWith(TEXT("PROTOTYPE_TOKEN_")))
    {
        FString SeedStr = SignedToken.RightChop(16);
        int32 Seed = FCString::Atoi(*SeedStr);
        int32 s = Seed;
        OutResult.DiceA = (s % 6) + 1;
        OutResult.DiceB = ((s / 7) % 6) + 1;
        OutResult.Outcome = OutResult.DiceA + OutResult.DiceB;
        OutResult.DebugInfo = TEXT("Prototype verified token");
        return true;
    }

    return false;
}

bool UMadCrapsRulesBridge::VerifySignedRollBlob(const FString& SerializedResult, const FString& SignatureBase64, const FString& PublicKeyBase64, FRollResult& OutResult)
{
    // Decode base64 signature and public key
    TArray<uint8> SigBytes;
    TArray<uint8> PubBytes;

    if (!FBase64::Decode(SignatureBase64, SigBytes))
    {
        UE_LOG(LogTemp, Warning, TEXT("VerifySignedRollBlob: failed to decode signature base64"));
        return false;
    }

    if (!FBase64::Decode(PublicKeyBase64, PubBytes))
    {
        UE_LOG(LogTemp, Warning, TEXT("VerifySignedRollBlob: failed to decode public key base64"));
        return false;
    }

    // Prepare message bytes as UTF-8 serialized JSON string
    FTCHARToUTF8 Converter(*SerializedResult);
    const uint8* MsgBytes = (const uint8*)Converter.Get();
    int MsgLen = Converter.Length();

    // Call native verifier (placeholder stub included). Replace with a real Ed25519 verifier linked from ThirdParty.
    bool ok = NativeVerifyEd25519(PubBytes.GetData(), PubBytes.Num(), SigBytes.GetData(), SigBytes.Num(), MsgBytes, MsgLen);
    if (!ok)
    {
        UE_LOG(LogTemp, Warning, TEXT("VerifySignedRollBlob: native verification failed"));
        return false;
    }

    // If verified, extract roll fields from JSON
    // For safety, parse JSON rather than re-serializing
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(SerializedResult);
    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        OutResult.DebugInfo = JsonObject->GetStringField(TEXT("debug_info"));
        OutResult.DiceA = JsonObject->GetIntegerField(TEXT("dice_a"));
        OutResult.DiceB = JsonObject->GetIntegerField(TEXT("dice_b"));
        OutResult.Outcome = JsonObject->GetIntegerField(TEXT("outcome"));
        return true;
    }

    UE_LOG(LogTemp, Warning, TEXT("VerifySignedRollBlob: failed to parse result JSON after verification"));
    return false;
}

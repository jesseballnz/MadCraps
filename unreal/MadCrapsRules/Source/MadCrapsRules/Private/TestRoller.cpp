#include "TestRoller.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "Json.h"
#include "RulesBridge.h"
#include "DiceActor.h"

ATestRoller::ATestRoller()
{
    PrimaryActorTick.bCanEverTick = false;
    ServerBaseUrl = TEXT("http://127.0.0.1:3000");
}

void ATestRoller::BeginPlay()
{
    Super::BeginPlay();
}

void ATestRoller::StartRoll(const FString& Nonce)
{
    FString Url = ServerBaseUrl + TEXT("/roll");

    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    if (!Nonce.IsEmpty())
    {
        JsonObject->SetStringField(TEXT("nonce"), Nonce);
    }

    FString Body;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetVerb(TEXT("POST"));
    Request->SetURL(Url);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(Body);
    Request->OnProcessRequestComplete().BindUObject(this, &ATestRoller::OnResponseReceived);
    Request->ProcessRequest();
}

void ATestRoller::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Roll request failed or no response"));
        return;
    }

    FString ResponseStr = Response->GetContentAsString();
    TSharedPtr<FJsonObject> RootObj;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
    if (!FJsonSerializer::Deserialize(Reader, RootObj) || !RootObj.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to parse roll response JSON"));
        return;
    }

    // Extract the serialized result - the server returns "result" object; we need the compact serialized string to verify.
    // We'll re-serialize the result object compactly to match server serialization.
    TSharedPtr<FJsonObject> ResultObj = RootObj->GetObjectField(TEXT("result"));
    FString SerializedResult;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&SerializedResult);
    FJsonSerializer::Serialize(ResultObj.ToSharedRef(), Writer);

    FString Signature = RootObj->GetStringField(TEXT("signature"));

    // Get public key from server (simpler to use /public_key)
    FString PubKeyUrl = ServerBaseUrl + TEXT("/public_key");
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> PubReq = FHttpModule::Get().CreateRequest();
    PubReq->SetVerb(TEXT("GET"));
    PubReq->SetURL(PubKeyUrl);

    // We'll do a synchronous-ish sequence: request public key, then verify; for simplicity, process in a nested callback.
    PubReq->OnProcessRequestComplete().BindLambda([this, SerializedResult, Signature, ResultObj](FHttpRequestPtr Req2, FHttpResponsePtr Resp2, bool bOk2)
    {
        if (!bOk2 || !Resp2.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to fetch public key"));
            return;
        }

        FString PubKeyB64 = Resp2->GetContentAsString();
        FRollResult OutResult;
        bool ok = UMadCrapsRulesBridge::VerifySignedRollBlob(SerializedResult, Signature, PubKeyB64, OutResult);
        if (!ok)
        {
            UE_LOG(LogTemp, Warning, TEXT("Signature verification failed"));
            return;
        }

        UE_LOG(LogTemp, Log, TEXT("Verified roll: A=%d B=%d Outcome=%d"), OutResult.DiceA, OutResult.DiceB, OutResult.Outcome);

        // Drive DiceActor if set
        if (DiceActor)
        {
            ADiceActor* DA = Cast<ADiceActor>(DiceActor);
            if (DA)
            {
                DA->PlayRollAnimation();
                DA->SnapToFaces(OutResult.DiceA, OutResult.DiceB);
            }
        }
    });

    PubReq->ProcessRequest();
}

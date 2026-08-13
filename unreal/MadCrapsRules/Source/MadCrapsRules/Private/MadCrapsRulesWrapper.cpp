#include "MadCrapsRulesWrapper.h"

#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#include "RulesEngine.h"

namespace
{
	using NativeBetType = madcraps::BetType;

	NativeBetType ToNativeBetType(const EMadCrapsBetType BetType)
	{
		switch (BetType)
		{
		case EMadCrapsBetType::PassLine: return NativeBetType::PassLine;
		case EMadCrapsBetType::DontPass: return NativeBetType::DontPass;
		case EMadCrapsBetType::Come: return NativeBetType::Come;
		case EMadCrapsBetType::DontCome: return NativeBetType::DontCome;
		case EMadCrapsBetType::Odds: return NativeBetType::Odds;
		case EMadCrapsBetType::Field: return NativeBetType::Field;
		case EMadCrapsBetType::Place: return NativeBetType::Place;
		case EMadCrapsBetType::Buy: return NativeBetType::Buy;
		case EMadCrapsBetType::Lay: return NativeBetType::Lay;
		case EMadCrapsBetType::Hardway: return NativeBetType::Hardway;
		case EMadCrapsBetType::Big6: return NativeBetType::Big6;
		case EMadCrapsBetType::Big8: return NativeBetType::Big8;
		case EMadCrapsBetType::Any7: return NativeBetType::Any7;
		case EMadCrapsBetType::AnyCraps: return NativeBetType::AnyCraps;
		case EMadCrapsBetType::Horn: return NativeBetType::Horn;
		case EMadCrapsBetType::Yo: return NativeBetType::Yo;
		case EMadCrapsBetType::AceDeuce: return NativeBetType::AceDeuce;
		default: return NativeBetType::PassLine;
		}
	}

	madcraps::Bet ToNativeBet(const FMadCrapsBet& Bet)
	{
		madcraps::Bet NativeBet;
		NativeBet.type = ToNativeBetType(Bet.Type);
		NativeBet.amount = Bet.Amount;
		NativeBet.target = Bet.Target;
		NativeBet.tag = TCHAR_TO_UTF8(*Bet.Tag);
		return NativeBet;
	}

	FMadCrapsRollResult ToBlueprintRoll(const madcraps::RollResult& Roll)
	{
		FMadCrapsRollResult Result;
		Result.Die1 = Roll.die1;
		Result.Die2 = Roll.die2;
		Result.Total = Roll.total();
		return Result;
	}

	madcraps::RollResult ToNativeRoll(const FMadCrapsRollResult& Roll)
	{
		return madcraps::RollResult{Roll.Die1, Roll.Die2};
	}

	FMadCrapsPayout ToBlueprintPayout(const madcraps::Payout& Payout)
	{
		FMadCrapsPayout Result;
		Result.Net = Payout.net;
		Result.Description = UTF8_TO_TCHAR(Payout.description.c_str());
		return Result;
	}

	void CopyNumericMap(const TMap<int32, double>& Source, std::map<int, double>& Target)
	{
		Target.clear();
		for (const TPair<int32, double>& Pair : Source)
		{
			Target.emplace(Pair.Key, Pair.Value);
		}
	}

	void CopyStringMap(const TMap<FString, double>& Source, std::map<std::string, double>& Target)
	{
		Target.clear();
		for (const TPair<FString, double>& Pair : Source)
		{
			Target.emplace(TCHAR_TO_UTF8(*Pair.Key), Pair.Value);
		}
	}

	void ReadIntDoubleMap(const TSharedPtr<FJsonObject>& JsonObject, const TCHAR* FieldName, TMap<int32, double>& OutMap)
	{
		OutMap.Reset();

		TSharedPtr<FJsonObject> MapObject;
		if (!JsonObject->TryGetObjectField(FieldName, MapObject) || !MapObject.IsValid())
		{
			return;
		}

		for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : MapObject->Values)
		{
			const int32 Key = FCString::Atoi(*Pair.Key);
			double Value = 0.0;
			if (Pair.Value.IsValid() && Pair.Value->TryGetNumber(Value))
			{
				OutMap.Add(Key, Value);
			}
		}
	}

	void ReadStringDoubleMap(const TSharedPtr<FJsonObject>& JsonObject, const TCHAR* FieldName, TMap<FString, double>& OutMap)
	{
		OutMap.Reset();

		TSharedPtr<FJsonObject> MapObject;
		if (!JsonObject->TryGetObjectField(FieldName, MapObject) || !MapObject.IsValid())
		{
			return;
		}

		for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : MapObject->Values)
		{
			double Value = 0.0;
			if (Pair.Value.IsValid() && Pair.Value->TryGetNumber(Value))
			{
				OutMap.Add(Pair.Key, Value);
			}
		}
	}
}

static madcraps::RulesEngine GMadCrapsRulesEngine;

UMadCrapsRulesWrapper::UMadCrapsRulesWrapper()
{
}

void UMadCrapsRulesWrapper::SetTableConfig(const FMadCrapsTableConfig& TableConfig)
{
	ApplyTableConfig(TableConfig);
}

bool UMadCrapsRulesWrapper::SetTableConfigFromJSON(const FString& JsonString)
{
	TSharedPtr<FJsonObject> JsonObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("MadCrapsRulesWrapper: failed to parse table config JSON"));
		return false;
	}

	FMadCrapsTableConfig TableConfig;
	JsonObject->TryGetNumberField(TEXT("field_two_payout"), TableConfig.FieldTwoPayout);
	JsonObject->TryGetNumberField(TEXT("field_twelve_payout"), TableConfig.FieldTwelvePayout);
	JsonObject->TryGetNumberField(TEXT("buy_commission_pct"), TableConfig.BuyCommissionPct);

	ReadIntDoubleMap(JsonObject, TEXT("odds_payout"), TableConfig.OddsPayout);
	ReadIntDoubleMap(JsonObject, TEXT("place_payout"), TableConfig.PlacePayout);
	ReadIntDoubleMap(JsonObject, TEXT("buy_payout"), TableConfig.BuyPayout);
	ReadIntDoubleMap(JsonObject, TEXT("lay_payout"), TableConfig.LayPayout);
	ReadIntDoubleMap(JsonObject, TEXT("hardway_payout"), TableConfig.HardwayPayout);
	ReadStringDoubleMap(JsonObject, TEXT("prop_payouts"), TableConfig.PropPayouts);

	ApplyTableConfig(TableConfig);
	return true;
}

void UMadCrapsRulesWrapper::Seed(const int64 Seed)
{
	GMadCrapsRulesEngine.seed(static_cast<uint64>(Seed));
}

FMadCrapsRollResult UMadCrapsRulesWrapper::RollDice()
{
	return ToBlueprintRoll(GMadCrapsRulesEngine.rollDice());
}

TArray<FMadCrapsPayout> UMadCrapsRulesWrapper::ResolveBets(const TArray<FMadCrapsBet>& Bets, const FMadCrapsRollResult& Roll, const int32 CurrentPoint)
{
	std::vector<madcraps::Bet> NativeBets;
	NativeBets.reserve(Bets.Num());

	for (const FMadCrapsBet& Bet : Bets)
	{
		NativeBets.push_back(ToNativeBet(Bet));
	}

	const std::vector<madcraps::Payout> NativePayouts =
		GMadCrapsRulesEngine.resolveBetsOnRoll(NativeBets, ToNativeRoll(Roll), CurrentPoint);

	TArray<FMadCrapsPayout> Results;
	Results.Reserve(static_cast<int32>(NativePayouts.size()));
	for (const madcraps::Payout& Payout : NativePayouts)
	{
		Results.Add(ToBlueprintPayout(Payout));
	}

	return Results;
}

TArray<FMadCrapsPayout> UMadCrapsRulesWrapper::ResolveRoll(const int32 Die1, const int32 Die2, const int32 CurrentPoint, const TArray<FMadCrapsBet>& Bets)
{
	FMadCrapsRollResult Roll;
	Roll.Die1 = Die1;
	Roll.Die2 = Die2;
	Roll.Total = Die1 + Die2;
	return ResolveBets(Bets, Roll, CurrentPoint);
}

void UMadCrapsRulesWrapper::ApplyTableConfig(const FMadCrapsTableConfig& TableConfig)
{
	madcraps::TableConfig NativeConfig;
	NativeConfig.field_two_payout = TableConfig.FieldTwoPayout;
	NativeConfig.field_twelve_payout = TableConfig.FieldTwelvePayout;
	NativeConfig.buy_commission_pct = TableConfig.BuyCommissionPct;

	CopyNumericMap(TableConfig.OddsPayout, NativeConfig.odds_payout);
	CopyNumericMap(TableConfig.PlacePayout, NativeConfig.place_payout);
	CopyNumericMap(TableConfig.BuyPayout, NativeConfig.buy_payout);
	CopyNumericMap(TableConfig.LayPayout, NativeConfig.lay_payout);
	CopyNumericMap(TableConfig.HardwayPayout, NativeConfig.hardway_payout);
	CopyStringMap(TableConfig.PropPayouts, NativeConfig.prop_payouts);

	GMadCrapsRulesEngine.setTableConfig(NativeConfig);
}

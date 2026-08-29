#include "MadCrapsRulesLibrary.h"
#include "MadCrapsTypes.h"

UMadCrapsGameState* UMadCrapsRulesLibrary::CreateGameState()
{
    UMadCrapsGameState* GameState = NewObject<UMadCrapsGameState>();
    if (GameState)
    {
        GameState->CurrentTableConfig.TableName = FString(TEXT("Vegas Classic"));
        GameState->CurrentTableConfig.Field2Payout = 2.0f;
        GameState->CurrentTableConfig.Field12Payout = 3.0f;
        GameState->CurrentPoint = 0;
    }
    return GameState;
}

void UMadCrapsRulesLibrary::SetTableConfig(UMadCrapsGameState* GameState, const FMadCrapsTableConfig& Config)
{
    if (GameState)
    {
        GameState->CurrentTableConfig = Config;
    }
}

FMadCrapsRollResult UMadCrapsRulesLibrary::RollDice(UMadCrapsGameState* GameState)
{
    FMadCrapsRollResult Result;
    if (GameState)
    {
        Result.Die1 = FMath::RandRange(1, 6);
        Result.Die2 = FMath::RandRange(1, 6);
    }
    return Result;
}

TArray<FMadCrapsPayout> UMadCrapsRulesLibrary::ResolveBetsOnRoll(
    UMadCrapsGameState* GameState,
    const TArray<FMadCrapsBet>& Bets,
    const FMadCrapsRollResult& Roll,
    int32 CurrentPoint)
{
    TArray<FMadCrapsPayout> Payouts;

    if (!GameState)
    {
        return Payouts;
    }

    int32 Total = Roll.Total();

    for (const FMadCrapsBet& Bet : Bets)
    {
        FMadCrapsPayout Payout;
        Payout.Net = 0.0f;
        Payout.Description = FString(TEXT("No resolution"));

        switch (Bet.Type)
        {
            case EMadCrapsBetType::PassLine:
            {
                if (CurrentPoint == 0)
                {
                    if (Total == 7 || Total == 11)
                    {
                        Payout.Net = Bet.Amount;
                        Payout.Description = FString(TEXT("PassLine win (come-out)"));
                    }
                    else if (Total == 2 || Total == 3 || Total == 12)
                    {
                        Payout.Net = -Bet.Amount;
                        Payout.Description = FString(TEXT("PassLine loss (craps)"));
                    }
                    else
                    {
                        Payout.Description = FString(TEXT("PassLine point established"));
                    }
                }
                else
                {
                    if (Total == CurrentPoint)
                    {
                        Payout.Net = Bet.Amount;
                        Payout.Description = FString(TEXT("PassLine win (point made)"));
                    }
                    else if (Total == 7)
                    {
                        Payout.Net = -Bet.Amount;
                        Payout.Description = FString(TEXT("PassLine loss (seven out)"));
                    }
                }
                break;
            }
            case EMadCrapsBetType::DontPass:
            {
                if (CurrentPoint == 0)
                {
                    if (Total == 2 || Total == 3)
                    {
                        Payout.Net = Bet.Amount;
                        Payout.Description = FString(TEXT("DontPass win (come-out)"));
                    }
                    else if (Total == 7 || Total == 11)
                    {
                        Payout.Net = -Bet.Amount;
                        Payout.Description = FString(TEXT("DontPass loss (come-out)"));
                    }
                    else if (Total == 12)
                    {
                        Payout.Net = 0.0f;
                        Payout.Description = FString(TEXT("DontPass push (bar 12)"));
                    }
                }
                else
                {
                    if (Total == 7)
                    {
                        Payout.Net = Bet.Amount;
                        Payout.Description = FString(TEXT("DontPass win (seven out)"));
                    }
                    else if (Total == CurrentPoint)
                    {
                        Payout.Net = -Bet.Amount;
                        Payout.Description = FString(TEXT("DontPass loss (point made)"));
                    }
                }
                break;
            }
            case EMadCrapsBetType::Field:
            {
                if (Total == 2)
                {
                    Payout.Net = Bet.Amount * GameState->CurrentTableConfig.Field2Payout;
                    Payout.Description = FString(TEXT("Field: 2"));
                }
                else if (Total == 12)
                {
                    Payout.Net = Bet.Amount * GameState->CurrentTableConfig.Field12Payout;
                    Payout.Description = FString(TEXT("Field: 12"));
                }
                else if (Total == 3 || Total == 4 || Total == 9 || Total == 10 || Total == 11)
                {
                    Payout.Net = Bet.Amount;
                    Payout.Description = FString(TEXT("Field: other wins"));
                }
                else
                {
                    Payout.Net = -Bet.Amount;
                    Payout.Description = FString(TEXT("Field: lose"));
                }
                break;
            }
            case EMadCrapsBetType::Any7:
            {
                if (Total == 7)
                {
                    Payout.Net = Bet.Amount * GameState->CurrentTableConfig.Any7Payout;
                    Payout.Description = FString(TEXT("Any 7 wins"));
                }
                else
                {
                    Payout.Net = -Bet.Amount;
                    Payout.Description = FString(TEXT("Any 7 loses"));
                }
                break;
            }
            case EMadCrapsBetType::AnyCraps:
            {
                if (Total == 2 || Total == 3 || Total == 12)
                {
                    Payout.Net = Bet.Amount * GameState->CurrentTableConfig.AnyCrapsPayout;
                    Payout.Description = FString(TEXT("Any Craps wins"));
                }
                else
                {
                    Payout.Net = -Bet.Amount;
                    Payout.Description = FString(TEXT("Any Craps loses"));
                }
                break;
            }
            default:
                Payout.Description = FString::Printf(TEXT("Bet type %d resolution stub"), static_cast<uint8>(Bet.Type));
                break;
        }

        Payouts.Add(Payout);
    }

    return Payouts;
}

float UMadCrapsRulesLibrary::CalculateHouseEdge(UMadCrapsGameState* GameState, EMadCrapsBetType BetType, int32 Trials)
{
    if (!GameState || Trials <= 0)
    {
        return 0.0f;
    }

    double NetWinnings = 0.0;

    for (int32 i = 0; i < Trials; ++i)
    {
        FMadCrapsRollResult Roll = RollDice(GameState);
        TArray<FMadCrapsBet> TestBets;
        TestBets.Add(FMadCrapsBet{BetType, 1.0f, 0, FString(TEXT(""))});

        TArray<FMadCrapsPayout> Payouts = ResolveBetsOnRoll(GameState, TestBets, Roll, 0);
        if (Payouts.Num() > 0)
        {
            NetWinnings += Payouts[0].Net;
        }
    }

    return static_cast<float>(NetWinnings / Trials);
}

void UMadCrapsRulesLibrary::SeedRNG(UMadCrapsGameState* GameState, int32 Seed)
{
    if (GameState)
    {
        FMath::RandInit(Seed);
    }
}

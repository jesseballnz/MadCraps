#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DiceActor.generated.h"

class UStaticMeshComponent;
class UMaterialInstanceDynamic;

/**
 * ADiceActor
 *
 * A single craps die with:
 *   - Beveled cube mesh with sharp pip indentations (assign SM_Dice in BP)
 *   - PBR material driven by a Dynamic Material Instance:
 *       BaseColor (ivory white), Roughness (0.05–0.12), Metallic (0),
 *       Specular (0.75), normal for sharp edges, emissive for face highlight
 *   - Physics simulation via UStaticMeshComponent primitive collision
 *   - SnapToFaces() — instantly rotates the mesh so the desired number
 *     faces upward (used after roll resolution from the server)
 *   - PlayRollAnimation() — applies a random tumbling rotation + impulse
 *     so players see the die spin before it snaps to its final face
 *   - Highlight(Face) — briefly illuminates the top face with a warm gold
 *     emissive pulse to emphasise the result
 *
 * The six face rotations are pre-computed in SnapToFaces() using standard
 * Western die orientation (opposite faces always sum to 7):
 *   Face 1 up = pitch 0,   yaw 0
 *   Face 2 up = pitch 0,   yaw 90
 *   Face 3 up = pitch -90, yaw 0
 *   Face 4 up = pitch 90,  yaw 0
 *   Face 5 up = pitch 0,   yaw -90
 *   Face 6 up = pitch 180, yaw 0
 *
 * Nanite note: enable Nanite on SM_Dice to get full pip detail without LOD cost.
 */
UCLASS(BlueprintType, Blueprintable)
class MADCRAPSRULES_API ADiceActor : public AActor
{
    GENERATED_BODY()

public:
    ADiceActor();

    // ------------------------------------------------------------------
    // Components
    // ------------------------------------------------------------------

    /** Assign SM_Dice (beveled cube, ~2 cm sides) in the Blueprint defaults. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dice")
    UStaticMeshComponent* DiceMesh;

    // ------------------------------------------------------------------
    // Configuration
    // ------------------------------------------------------------------

    /** Die body colour (ivory white by default, or custom per theme). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice|Material")
    FLinearColor DiceBodyColor = FLinearColor(0.92f, 0.92f, 0.88f, 1.f);

    /** Pip colour (classic: red for 1, black for 2–6; or white for modern sets). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice|Material")
    FLinearColor PipColor = FLinearColor(0.05f, 0.05f, 0.05f, 1.f);

    /** Material roughness — lower = more reflective (0.05 recommended for casino dice). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice|Material",
              meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float DiceRoughness = 0.07f;

    /** Duration (s) of the roll-spin animation before snapping to the final face. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice|Animation",
              meta = (ClampMin = "0.1", ClampMax = "3.0"))
    float RollAnimDuration = 0.8f;

    /** How long (s) to hold the face-highlight emissive pulse after snapping. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice|Animation",
              meta = (ClampMin = "0.1", ClampMax = "3.0"))
    float FaceHighlightDuration = 1.2f;

    // ------------------------------------------------------------------
    // Blueprint callable API
    // ------------------------------------------------------------------

    /**
     * Snap the mesh to show Face value on top (1–6).
     * The second parameter (ignored in single-die use) is retained for
     * backward compatibility with the previous two-die API.
     */
    UFUNCTION(BlueprintCallable, Category = "Dice")
    void SnapToFaces(int32 FaceUp, int32 Unused = 0);

    /**
     * Play a brief tumbling animation.  When complete, calls SnapToFaces
     * with PendingFace (set before calling if the face is known ahead of time).
     */
    UFUNCTION(BlueprintCallable, Category = "Dice")
    void PlayRollAnimation();

    /**
     * Set the face this die will display after the roll animation completes.
     * Call this when the server-authoritative result arrives.
     */
    UFUNCTION(BlueprintCallable, Category = "Dice")
    void SetPendingFace(int32 Face);

    /**
     * Returns the face currently showing on top (1–6, or 0 if not set).
     */
    UFUNCTION(BlueprintPure, Category = "Dice")
    int32 GetCurrentFace() const { return CurrentFace; }

    /** Brief gold emissive pulse on the top face. */
    UFUNCTION(BlueprintCallable, Category = "Dice")
    void HighlightTopFace();

    // ------------------------------------------------------------------
    // AActor overrides
    // ------------------------------------------------------------------

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    /** Pre-computed face rotations: Index 0 = face 1 up, ... Index 5 = face 6 up. */
    static const FRotator FaceRotations[6];

    void InitMaterial();

    UPROPERTY()
    UMaterialInstanceDynamic* DiceMID;

    int32 CurrentFace  = 0;
    int32 PendingFace  = 1;

    // Animation state
    bool  bRolling          = false;
    float RollAnimTimer     = 0.f;
    bool  bHighlighting     = false;
    float HighlightTimer    = 0.f;
    FRotator RollStartRotation;
    FRotator RollSpinSpeed;   // random tumble speed
};

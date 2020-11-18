Slate Data Binding Part 3

## Step 1: Overview & Prep
What is data binding? Data binding is a concept from software development where information output, such as the player's current health, is tied to the information it actually represents. In this way, any time you change the data (for example, by damaging the player) the display is updated automatically. This can have a great many uses, both simple and complex, in Soul Tech, a Mech Action/RPG I am programming for, I use data binding handle the individual pages available in the Mech customization & shop screens. My UI only has to worry about having space to display data from the current page, updating the actual page does not require also sending update messages to the UI.

In our case, we are going to create a new Slate UI that will serve as an in-game HUD, displaying the player's current health and score in the upper corners of the screen. Initially, the HUD will only have static values, we will change it to be bound data in the next step. I'm going to paste the code below, but I will not go into detail on how it works, it's all pretty simple content that has been covered in past tutorials.

TutorialGameHUDUI.h
```
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
// TutorialGameHUDUI.h - Provides an implementation of the Slate UI representing the tutorial game HUD.

#pragma once

#include "Slate.h"

// Lays out and controls the Tutorial HUD UI.

class STutorialGameHUDUI : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(STutorialGameHUDUI)
        : _OwnerHUD()
    {
    }

    SLATE_ARGUMENT(TWeakObjectPtr, OwnerHUD);

    SLATE_END_ARGS()

public:
    /**
     * Constructs and lays out the Tutorial HUD UI Widget.
     * 
     * \args Arguments structure that contains widget-specific setup information.
     **/
    void Construct(const FArguments& args);

private:
    /**
     * Stores a weak reference to the HUD owning this widget.
     **/
    TWeakObjectPtr OwnerHUD;

    /**
     * A reference to the Slate Style used for this HUD's widgets.
     **/
    const struct FGlobalStyle* HUDStyle;
};
```
TutorialGameHUDUI.cpp
```
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SlateTutorials.h"

#include "TutorialGameHUD.h"
#include "TutorialGameHUDUI.h"

#include "Menus/GlobalMenuStyle.h"
#include "Menus/MenuStyles.h"

void STutorialGameHUDUI::Construct(const FArguments& args)
{
    OwnerHUD = args._OwnerHUD;

    HUDStyle = &FMenuStyles::Get().GetWidgetStyle<FGlobalStyle>("Global");

    ChildSlot
        [
            SNew(SOverlay)
            + SOverlay::Slot()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Top)
                [
                    SNew(STextBlock)
                        .TextStyle(&HUDStyle->MenuTitleStyle)
                        .Text(FText::FromString("SCORE: 0"))
                ]
            + SOverlay::Slot()
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Top)
                [
                    SNew(STextBlock)
                        .TextStyle(&HUDStyle->MenuTitleStyle)
                        .Text(FText::FromString("HEALTH: 100"))
                ]
        ];
}
```
TutorialGameHUD.h
```
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
// TutorialGameHUD.h - Provides an implementation of the HUD that will embed the Tutorial Game UI.

#pragma once

#include "GameFramework/HUD.h"

#include "TutorialGameHUD.generated.h"

/**
 * Provides an implementation of the game's in-game HUD, which will display the player's current health and score.
 **/
UCLASS()
class ATutorialGameHUD : public AHUD
{
    GENERATED_UCLASS_BODY()

public:
    /**
     * Initializes the Slate UI and adds it as a widget to the game viewport.
     **/
    virtual void PostInitializeComponents() override;

private:
    /**
     * Reference to the Game HUD UI.
     **/
    TSharedPtr GameHUD;
};
```
TutorialGameHUD.cpp
```
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SlateTutorials.h"

#include "TutorialGameHUD.h"
#include "TutorialGameHUDUI.h"

ATutorialGameHUD::ATutorialGameHUD(const class FPostConstructInitializeProperties& PCIP)
    : Super(PCIP)
{
}

void ATutorialGameHUD::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (GEngine && GEngine->GameViewport)
    {
        UGameViewportClient* Viewport = GEngine->GameViewport;

        SAssignNew(GameHUD, STutorialGameHUDUI)
            .OwnerHUD(TWeakObjectPtr(this));

        Viewport->AddViewportWidgetContent(
            SNew(SWeakWidget).PossiblyNullContent(GameHUD.ToSharedRef())
            );
    }
}
```
Go ahead and build the project and set up a new map and game mode and test the HUD out!

## Step 2: Binding our Data
We have two bits of information that we want to bind to our UI: Score and Health. Both of these are integers, but they have to be bound as strings for our HUD! I will cover obtaining this information from the Game Mode (Score) and active Character (Health) in a minute, but first we'll handle the data binding portion. Our binding is going to have two important tasks: first, it is going to obtain the actual data. Next, it will convert it to an FText to be applied to the text block widgets.

There are two things needed for each of these: the Attribute, and something to bind it to. In our case, because we need to do extra processing on the data (converting from an integer to a string), we will have a function on our widget class itself to bind to. Add the following private values to the STutorialGameHUDUI class:
```
private:
    /**
     * Attribute storing the binding for the player's score.
     **/
    TAttribute<FText> Score;

    /**
     * Attribute storing the binding for the player's health.
     **/
    TAttribute<FText> Health;

    /**
     * Our Score will be bound to this function, which will retrieve the appropriate data and convert it into an FText.
     **/
    FText GetScore() const;

    /**
     * Our Health will be bound to this function, which will retrieve the appropriate data and convert it into an FText.
     **/
    FText GetHealth() const;
```
The TAttribute type is used in Unreal to provide data binding that only has an accessor/getter. Next, we have two constant functions that will be responsible for retrieving and formatting the data into a form the UI can use! So how do we actually do the binding? Well, it's quite simple, in fact, you've already done it if you've ever done input binding within C++ for an Unreal project. In the top of STutorialGameHUDUI's Construct method, just after capturing HUDStyle, add the following to bind our TAttributes to their appropriate functions:
```
Score.Bind(this, &STutorialGameHUDUI::GetScore);
Health.Bind(this, &STutorialGameHUDUI::GetHealth);
```
Next, we can use the TAttributes directly in our Text specifications when constructing the UI layout:
```
+ SOverlay::Slot()
    .HAlign(HAlign_Right)
    .VAlign(VAlign_Top)
    [
        SNew(STextBlock)
            .TextStyle(&HUDStyle->MenuTitleStyle)
            .Text(Score)
    ]
+ SOverlay::Slot()
    .HAlign(HAlign_Left)
    .VAlign(VAlign_Top)
    [
        SNew(STextBlock)
            .TextStyle(&HUDStyle->MenuTitleStyle)
            .Text(Health)
    ]
```
And finally, we'll put in some placeholder data for our bound functions, just to make sure everything's worked:
```
FText STutorialGameHUDUI::GetScore() const { return FText::FromString("SCORE: --"); }
FText STutorialGameHUDUI::GetHealth() const { return FText::FromString("HEALTH: --"); }
```
Go ahead and build and make sure everythingÔÇÖs working, and congratulations! YouÔÇÖve just bound your text blocks! You can do this binding for just about everything in Slate ÔÇô button text, list items, image backgrounds, styles, etc.

## Step 3: Binding it to something Useful
If you only needed to learn how to do data binding, and don't care about the specifics for this tutorial then you don't need to continue past this point, here, we're just implementing our Score and Health functions. Consider it bonus content!

To get the score and health data for this tutorial, I've added the following GameMode and Character classes and set them up to be used with the GameMap level.

TutGameMode.h
```
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
// TutGameMode.h - Provides a simple game mode providing a Score!

#pragma once

#include "GameFramework/GameMode.h"
#include "TutGameMode.generated.h"

/**
 * A simple game mode providing a means of retrieving and adjusting a single Score value.
 **/
UCLASS()
class ATutGameMode : public AGameMode
{
    GENERATED_UCLASS_BODY()

public:
    /**
     * Retrieves the current Score from the game mode.
     **/
    UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Score")
    int32 GetScore();

    /**
     * Adds to the game score.
     **/
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddPoints(int32 value);

    /**
     * Removes from the game score.
     **/
    UFUNCTION(BlueprintCallable, Category = "Score")
    void DeductPoints(int32 value);

private:
    /**
     * Stores the current score.
     **/
    int32 CurrentScore;
};
```
TutGameMode.cpp
```
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SlateTutorials.h"

#include 

#include "TutGameMode.h"

ATutGameMode::ATutGameMode(const class FPostConstructInitializeProperties& PCIP)
    : Super(PCIP), CurrentScore(0)
{
}

int32 ATutGameMode::GetScore()
{
    return CurrentScore;
}

void ATutGameMode::AddPoints(int32 value)
{
    if (value > 0)
        CurrentScore += value;
}

void ATutGameMode::DeductPoints(int32 value)
{
    if (value > 0)
        CurrentScore = std::max(CurrentScore - value, 0);
}
```
TutorialCharacter.h
```
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
// TutorialCharacter.h - Provides a simple character providing Health!

#pragma once

#include "GameFramework/Character.h"
#include "TutorialCharacter.generated.h"

/**
 * A simple character providing a means of retrieving and manipulating health.
 **/
UCLASS()
class ATutorialCharacter : public ACharacter
{
    GENERATED_UCLASS_BODY()

public:
    /**
     * Stores the character's current health.
     **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 Health;
};
```
TutorialCharacter.cpp
```
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SlateTutorials.h"

#include "TutorialCharacter.h"

ATutorialCharacter::ATutorialCharacter(const class FPostConstructInitializeProperties& PCIP)
    : Super(PCIP)
{
    Health = 100;
}
```
Once we've got these, retrieving and applying the score and health is simple

TutorialGameHUDUI.cpp
```
FText STutorialGameHUDUI::GetScore() const
{
    // NOTE: THIS IS A TERRIBLE WAY TO DO THIS. DO NOT DO IT. IT ONLY WORKS ON SERVERS. USE GAME STATES INSTEAD!
    auto gameMode = Cast<ATutGameMode>(OwnerHUD->GetWorldSettings()->GetWorld()->GetAuthGameMode());

    if (gameMode == nullptr)
        return FText::FromString(TEXT("SCORE: --"));

    FString score = TEXT("SCORE: ");
    score.AppendInt(gameMode->GetScore());

    return FText::FromString(score);
}

FText STutorialGameHUDUI::GetHealth() const 
{
    ATutorialCharacter* character = Cast(OwnerHUD->PlayerOwner->GetCharacter());

    if (character == nullptr)
        return FText::FromString(TEXT("HEALTH: --"));

    FString health = TEXT("HEALTH: ");
    health.AppendInt(character->Health);

    return FText::FromString(health);
}
```
Go ahead and run the game, then update the score! (You can do this in the sample files by pressing the Home/End to adjust health & Page Up/Page Down to adjust score).

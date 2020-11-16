# UE4 Slate的使用示例流程

1. 添加Slate模块
打开对应工程.build.cs文件，添加Slate模块
```
PrivateDependencyModuleNames.AddRange(new string[] {
    "Slate",
    "SlateCore"
});
```
2. 创建一个继承自HUD的C++类AMenuHUD
此类将作为我们自定义的GameMode下的UI显示，并在类中定义我们需要显示的窗口控件（Slate Widget）。

//AMenuHUD.h:
```
UCLASS()
class MENU_API AMenuHUD : public AHUD
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
protected:
	TSharedPtr<class SMainMenuWidget> MenuWidget;	//要显示的菜单窗口
	TSharedPtr<class SWidget> MenuWidgetContainer;

public:
	void ShowMenu();	//显示菜单
	void RemoveMenu();	//移除菜单
};
```

//AMenuHUD.cpp:
```
void AMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	ShowMenu();
}

void AMenuHUD::ShowMenu()
{
	if (GEngine && GEngine->GameViewport)
	{
		MenuWidget = SNew(SMainMenuWidget).OwningHUD(this);	//把本身这个HUD类作为参数传给窗口类
		GEngine->GameViewport->AddViewportWidgetContent(SAssignNew(MenuWidgetContainer, SWeakWidget).PossiblyNullContent(MenuWidget.ToSharedRef()));
	}
	/* PlayerController which owns this HUD. */
	if (PlayerOwner)
	{
		PlayerOwner->bShowMouseCursor = true;
		PlayerOwner->SetInputMode(FInputModeUIOnly());	//设置输入模式
	}
}

void AMenuHUD::RemoveMenu()
{
	if (GEngine && GEngine->GameViewport && MenuWidgetContainer.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(MenuWidgetContainer.ToSharedRef());

		if (PlayerOwner)
		{
			PlayerOwner->bShowMouseCursor = false;
			PlayerOwner->SetInputMode(FInputModeGameAndUI());
		}
	}
}
```
3. 创建一个继承自Slate Widget的C++类SMainMenuWidget

在Construct函数里设计我们需要显示的控件。添加一个参数用来保存自己所属的HUD，需要通过HUD中的PlayerOwner变量来获取玩家控制器。
```
//SMainMenuWidget.h
class SMainMenuWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMainMenuWidget) {}

	SLATE_ARGUMENT(TWeakObjectPtr<class AMenuHUD>, OwningHUD);

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual bool SupportsKeyboardFocus() const override { return true; }

	FReply OnPlayClicked() const;	//开始按钮点击事件
	FReply OnQuitClicked() const;	//退出按钮点击事件

private:
	TWeakObjectPtr<class AMenuHUD> OwningHUD;
};

void SMainMenuWidget::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	OwningHUD = InArgs._OwningHUD;	//将生成此窗口的HUD类作为参数传过来（必须加下划线_）
	// 文本和按钮间距设置
	const FMargin ContentPadding = FMargin(500.0f, 300.0f);
	const FMargin ButtonPadding = FMargin(10.f);
	// 按钮和标题文本
	const FText TitleText = LOCTEXT("GameTitle", "My Super Great Game");
	const FText PlayText = LOCTEXT("PlayGame", "Play");
	const FText QuitText = LOCTEXT("QuitGame", "Quit Game");
	//按钮字体及大小设置
	FSlateFontInfo ButtonTextStyle = FCoreStyle::Get().GetFontStyle("EmbossedText");
	ButtonTextStyle.Size = 40.f;
	//标题字体及大小设置
	FSlateFontInfo TitleTextStyle = ButtonTextStyle;
	TitleTextStyle.Size = 60.f;
	
	//所有UI控件都写在这里
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		[
			SNew(SImage)	// 背景图片（纯黑）
			.ColorAndOpacity(FColor::Black)
		]

		+ SOverlay::Slot()
		.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		.Padding(ContentPadding)
		[
			SNew(SVerticalBox)

			// Title Text
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Font(TitleTextStyle)
				.Text(TitleText)
				.Justification(ETextJustify::Center)
			]

			// Play Button
			+ SVerticalBox::Slot()
			.Padding(ButtonPadding)
			[
				SNew(SButton)
				.OnClicked(this, &SMainMenuWidget::OnPlayClicked)
				[
					SNew(STextBlock)
					.Font(ButtonTextStyle)
					.Text(PlayText)
					.Justification(ETextJustify::Center)
				]
			]

			// Quit Button
			+ SVerticalBox::Slot()
			.Padding(ButtonPadding)
			[
				SNew(SButton)
				.OnClicked(this, &SMainMenuWidget::OnQuitClicked)
				[
					SNew(STextBlock)
					.Font(ButtonTextStyle)
					.Text(QuitText)
					.Justification(ETextJustify::Center)
				]
			]
		]
	];
}

FReply SMainMenuWidget::OnPlayClicked() const
{
	if (OwningHUD.IsValid())
	{
		OwningHUD->RemoveMenu();
	}

	return FReply::Handled();
}

FReply SMainMenuWidget::OnQuitClicked() const
{
	if (OwningHUD.IsValid())
	{
		if (APlayerController* PC = OwningHUD->PlayerOwner)
		{
			PC->ConsoleCommand("quit");
		}
	}

	return FReply::Handled();
}

4 创建一个继承自PlayerController的C++类
实现按键弹出菜单的功能。

//MenuPlayerController.h:
```
UCLASS()
class MENU_API AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;
private:
	void OpenMenu();	
};
```
//MenuPlayerController.cpp:
```
void AMenuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		//在项目设置里设置对应的按键
		InputComponent->BindAction("OpenMenu", IE_Pressed, this, &AMenuPlayerController::OpenMenu);
	}
}

void AMenuPlayerController::OpenMenu()
{
	if (AMenuHUD* MenuHUD = GetHUD<AMenuHUD>())
	{
		MenuHUD->ShowMenu();
	}
}

5. 创建一个继承自Game Mode Base的C++类AMenuGameMode
在构造函数中初始化我们自定义的HUD类和PlayerController类
```
AMenuGameMode::AMenuGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	//初始化自定义类
	PlayerControllerClass = AMenuPlayerController::StaticClass();
	HUDClass = AMenuHUD::StaticClass();
}
```
使用Slate来写界面的大体流程就是这样。

注意：之前在获取玩家控制器的时候用UGameplayStatics::GetPlayerController(GWorld, 0)，但是返回的是null。所以如果要获取到玩家控制器，可以使用HUDClass里的PlayerOwner变量。

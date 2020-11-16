# UE4 Slate��ʹ��ʾ������

1. ���Slateģ��
�򿪶�Ӧ����.build.cs�ļ������Slateģ��
```
PrivateDependencyModuleNames.AddRange(new string[] {
    "Slate",
    "SlateCore"
});
```
2. ����һ���̳���HUD��C++��AMenuHUD
���ཫ��Ϊ�����Զ����GameMode�µ�UI��ʾ���������ж���������Ҫ��ʾ�Ĵ��ڿؼ���Slate Widget����

//AMenuHUD.h:
```
UCLASS()
class MENU_API AMenuHUD : public AHUD
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
protected:
	TSharedPtr<class SMainMenuWidget> MenuWidget;	//Ҫ��ʾ�Ĳ˵�����
	TSharedPtr<class SWidget> MenuWidgetContainer;

public:
	void ShowMenu();	//��ʾ�˵�
	void RemoveMenu();	//�Ƴ��˵�
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
		MenuWidget = SNew(SMainMenuWidget).OwningHUD(this);	//�ѱ������HUD����Ϊ��������������
		GEngine->GameViewport->AddViewportWidgetContent(SAssignNew(MenuWidgetContainer, SWeakWidget).PossiblyNullContent(MenuWidget.ToSharedRef()));
	}
	/* PlayerController which owns this HUD. */
	if (PlayerOwner)
	{
		PlayerOwner->bShowMouseCursor = true;
		PlayerOwner->SetInputMode(FInputModeUIOnly());	//��������ģʽ
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
3. ����һ���̳���Slate Widget��C++��SMainMenuWidget

��Construct���������������Ҫ��ʾ�Ŀؼ������һ���������������Լ�������HUD����Ҫͨ��HUD�е�PlayerOwner��������ȡ��ҿ�������
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

	FReply OnPlayClicked() const;	//��ʼ��ť����¼�
	FReply OnQuitClicked() const;	//�˳���ť����¼�

private:
	TWeakObjectPtr<class AMenuHUD> OwningHUD;
};

void SMainMenuWidget::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	OwningHUD = InArgs._OwningHUD;	//�����ɴ˴��ڵ�HUD����Ϊ������������������»���_��
	// �ı��Ͱ�ť�������
	const FMargin ContentPadding = FMargin(500.0f, 300.0f);
	const FMargin ButtonPadding = FMargin(10.f);
	// ��ť�ͱ����ı�
	const FText TitleText = LOCTEXT("GameTitle", "My Super Great Game");
	const FText PlayText = LOCTEXT("PlayGame", "Play");
	const FText QuitText = LOCTEXT("QuitGame", "Quit Game");
	//��ť���弰��С����
	FSlateFontInfo ButtonTextStyle = FCoreStyle::Get().GetFontStyle("EmbossedText");
	ButtonTextStyle.Size = 40.f;
	//�������弰��С����
	FSlateFontInfo TitleTextStyle = ButtonTextStyle;
	TitleTextStyle.Size = 60.f;
	
	//����UI�ؼ���д������
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		[
			SNew(SImage)	// ����ͼƬ�����ڣ�
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

4 ����һ���̳���PlayerController��C++��
ʵ�ְ��������˵��Ĺ��ܡ�

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
		//����Ŀ���������ö�Ӧ�İ���
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

5. ����һ���̳���Game Mode Base��C++��AMenuGameMode
�ڹ��캯���г�ʼ�������Զ����HUD���PlayerController��
```
AMenuGameMode::AMenuGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	//��ʼ���Զ�����
	PlayerControllerClass = AMenuPlayerController::StaticClass();
	HUDClass = AMenuHUD::StaticClass();
}
```
ʹ��Slate��д����Ĵ������̾���������

ע�⣺֮ǰ�ڻ�ȡ��ҿ�������ʱ����UGameplayStatics::GetPlayerController(GWorld, 0)�����Ƿ��ص���null���������Ҫ��ȡ����ҿ�����������ʹ��HUDClass���PlayerOwner������

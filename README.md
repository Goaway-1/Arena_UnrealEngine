# ArenaBattle

## **05.28**

> **<h3>Today Dev Story</h3>**

- ### Rider을 사용하여 자동 완성기능을 구현 (기존 제공은 느리다.)

- ### 고정된 분수대의 구현
   - <img src="Image/Fountain.gif" height="300" title="Fountain"> <img src="Image/Asset_Auto_Set.gif" height="300" title="Asset_Auto_Set">
   - 3개의 컴포넌트(스태딕 메시, 라이트, 파티클)을 할당하여 구현
   - 에셋의 경로를 컴포넌트로 바로 지정하여 생성가능하게 구현
      <details><summary>코드 보기</summary>

      ```c++
      //Fountain.h
      UPROPERTY(VisibleAnywhere)	//VisibleAnywhere->속성편집 가능하게 함, UPROPERTY->엔진이 컨트롤
      UStaticMeshComponent *Body;

      UPROPERTY(VisibleAnywhere)
      UStaticMeshComponent *Water;

      UPROPERTY(VisibleAnywhere)
      UPointLightComponent *Light;

      UPROPERTY(VisibleAnywhere)
      UParticleSystemComponent *Splash;

      UPROPERTY(EditAnywhere, Category=ID)
      int32 ID;
      //Fountain.cpp

      //컴포넌트를 생성하는 코드
         Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
      Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));
      Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("LIGHT"));
      Splash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SPLASH"));

      //Body가 루트컴포넌트가 되며 Water는 Body의 자식이 된다.
      RootComponent = Body;
      Water->SetupAttachment(Body);
      Light->SetupAttachment(Body);
      Splash->SetupAttachment(Body);

      //좌표값 설정
      Water->SetRelativeLocation(FVector(0.0f,0.0f,135.0f));
      Light->SetRelativeLocation(FVector(0.0f,0.0f,195.0f));
      Splash->SetRelativeLocation(FVector(0.0f,0.0f,195.0f));

      //오브젝트의 키값을 가져와서 성공했다면 Body에 대입
      static ConstructorHelpers::FObjectFinder<UStaticMesh>
      SM_BODY(TEXT("/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01"));

      if(SM_BODY.Succeeded()) Body->SetStaticMesh(SM_BODY.Object);

      static ConstructorHelpers::FObjectFinder<UStaticMesh>
      SM_WATER(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Meshes/Env/SM_Plains_Fountain_02.SM_Plains_Fountain_02"));

      if(SM_WATER.Succeeded()) Water->SetStaticMesh(SM_WATER.Object);

      static ConstructorHelpers::FObjectFinder<UParticleSystem>
      SM_SPLASH(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Ambient/Water/P_Water_Fountain_Splash_Base_01.P_Water_Fountain_Splash_Base_01"));

      if(SM_SPLASH.Succeeded()) Splash->SetTemplate(SM_SPLASH.Object);
      ```

      </details>

- ### 움직이는 액터 설계
   - <img src="Image/Fountain_Rotate.gif" height="300" title="  Fountain_Rotate">
   1. 매크로를 사용하여 구현
      - Tick에서 구현하고 DeltaTime을 사용한다.
      - 컴포넌트를 추가하지 않아도 된다.
         <details><summary>코드 보기</summary>

         ```c++
         //Fountain.h
         private:
            UPROPERTY(EditAnywhere, Category=stat, Meta=(AllowPrivateAccess= true)) //private 타입이지만 Meta를 추가하여 접근가능
            float RotateSpeed;
         //Fountain.cpp
         AFountain::AFountain()
         {
            ...
            RotateSpeed = 30.0f;
         }
         void AFountain::Tick(float DeltaTime)
         {
            Super::Tick(DeltaTime);
            AddActorLocalRotation(FRotator(0.0f,RotateSpeed * DeltaTime,0.0f));  // Y, Z , X축 순서
         }
         ```

         </details>
      

   2. 무브먼트 컴포넌트 사용
      - Tick(Update)에서 사용하지 않아도 되고, DeltaTime을 사용하지 않는다.
      - 아래와 같이 다양한 움직임을 구사한다.
         - FloatingPawnMovement : 중력에 영향을 받지 않는 액터
         - RotatingMovement : 액터를 회전
         - InterpMovement : 액터를 지정된 좌표로 이동
         - ProjectMovement : 액터에 중력의 영향을 받아 포물선을 그리는 발사체 움직임

      <details><summary>코드 보기</summary>

      ```c++
      //Fountain.h
      #include "GameFramework/RotatingMovementComponent.h"
      ...
      UPROPERTY(VisibleAnywhere)
	   URotatingMovementComponent *Movement;
      //Fountain.cpp
	   Movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("MOVEMENT"));
	   Movement->RotationRate = FRotator(0.0f,RotateSpeed,0.0f);
      ```

      </details>

- ### 로그의 출력 방법
   - <img src="Image/Set_Log.png" height="300" title="Set_Log">
   - 게임 모듈에 로그를 추가하고 다른 모듈에서 끌어다 쓴다.
   - 그렇기에 다른 모듈(cpp)에는 항상 게임 모듈을 넣어둔다.

      <details><summary>코드 보기</summary>

      ```c++
      //Arena.cpp
      #include "Arena.h"
      #include "Modules/ModuleManager.h"

      DEFINE_LOG_CATEGORY(Arena);   //추가된 부분
      IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Arena, "Arena" );

      //Arena.h
      #pragma once

      #include "EngineMinimal.h"       //추가된 부분

      DECLARE_LOG_CATEGORY_EXTERN(Arena, Log, All);   //추가된 부분
      //메크로 선언
      #define ABLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
      #define ABLOG_S(Verbosity) UE_LOG(Arena, Verbosity, TEXT("%s"), *ABLOG_CALLINFO)
      #define ABLOG(Verbosity,Format,...) UE_LOG(Arena, Verbosity, TEXT("%s %s"),*ABLOG_CALLINFO,*FString::Printf(Format, ##__VA_ARGS__))

      //Fountain.h
      #include "Arena.h" //기존 "EngineMinmal.h"가 아닌 "Arena.h"참조

      //Fountain.cpp
      void AFountain::BeginPlay()
      {
         Super::BeginPlay();

         //UE_LOG(Arena,Warning,TEXT("Actor Name : %s, ID : %d, Location X : %.3f"),*this->GetName(),ID,GetActorLocation().X); -> 이전 방식
         //최근 방식
         ABLOG_S(Warning);
         ABLOG(Warning, TEXT("Actor Name : %s, ID : %d, Location X : %.3f"),*this->GetName(),ID,GetActorLocation().X);
      }
      ```

      </details>


- ### GameMode & Pawn & PlayerController 설정
   - <img src="Image/GameMode_set.png" height="300" title="GameMode_set"> 
   - ABGameMode, ABPawn, ABPlayerController을 생성하고 ABGameMode를 월드세팅에 GameMode에 넣어 룰을 지정한다.
   - ABPawn을 ABGameMode의 Default Pawn으로 지정한다. ABPlayerController또한 마찬가지 이다. (이때 생성이 아닌 클래스의 정보를 저장하는 것)
      <details><summary>코드 보기</summary>

      ```c++
      //ABGameMode.h
      class ARENA_API AABGameMode : public AGameModeBase
      {
         GENERATED_BODY()
      public:
         AABGameMode();
      };
      //ABGameMode.cpp
      #include "ABGameMode.h"
      #include "ABPawn.h"	      //내가 참조할것
      #include "ABPlayerController.h"

      AABGameMode::AABGameMode()
      {
         DefaultPawnClass = AABPawn::StaticClass();	//ABPawn의 클래스 정보를 저장 (멀티 플레이를 고려, 만들어두는 것이 아님)
         PlayerControllerClass = AABPlayerController::StaticClass();
      }
      ```

      </details>

- ### 플레이어의 입장
   - <img src="Image/Player_Login.png" height="300" title="Player_Login"> 
   - 플레이어가 로그인(입장)하면 언리얼 엔진에서 PostLogin 이벤트 함수가 호출된다. 
      - 이 과정에서 플레이어가 조종한 폰을 생성하고 플레이어 컨트롤러가 해당 폰에 빙의하는 작업이 이루어진다.
   - 각 생성되는 시점은 PostInitializeComponents, 빙의를 진행하는 시점은 PalyerController의 Posses, Pawn의 PossesedBy로 알 수 있다.
   - 아래는 PostLogin 이벤트 함수에 로그를 찍어서 플레이어 설정이 이루어지는 과정을 나타냈다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABGameMode.h
      class ARENA_API AABGameMode : public AGameModeBase
      {
         GENERATED_BODY()
      public:
         AABGameMode();
      };
      //ABGameMode.cpp
      #include "ABGameMode.h"
      #include "ABPawn.h"	      //내가 참조할것
      #include "ABPlayerController.h"

      AABGameMode::AABGameMode()
      {
         DefaultPawnClass = AABPawn::StaticClass();	//ABPawn의 클래스 정보를 저장 (멀티 플레이를 고려, 만들어두는 것이 아님)
         PlayerControllerClass = AABPlayerController::StaticClass();
      }
      ```

      </details>
      
> **<h3>Realization</h3>**

- 유니티와 다르게 종류에 따라서 C++이 자동으로 생성된다.
- 포인터로 선언된 객체는 "UPROPERTY" 매크로를 사용하여 엔진이 스스로 컨트롤 하게 한다.
- 언리얼 오브젝트 클래스만 가능하며 아래와 같은 규칙이 존재한다.
  1. 클래스 선언 맥크로
     - 클래스 선언의 윗줄에 UCLASS 매크로 선언
     - 클래스 내부에는 GENERATED_BODY 매크로 선언
  2. 클래스 이름 접두사
     - 접두사가 A인 경우는 액터 클래스, 그 외에는 U라고 이름을 선언
     - 접두사가 F인 경우는 일반 C++ 클래스이다.
  3. generated.h 파일
     - 클래스를 분석하기 위함. 필수
  4. 외부 모듈에의 공개 여부
     - 클래스옆에 '모듈명\_API'를 선언해주어야 접근 가능
- 컴파일에 너무 많은 시간이 소요된다.
- 에셋을 자동호출하기 위해서 주소를 키값으로 사용하며 초기화의 필요가 없음으로 static으로 사용한다.
- 로그는 게임모듈명에서 선언하고 다른 모듈들이 이 헤더를 참조하게 만든다.
- 액터의 이벤트 함수 : BeginPlay, Tick, EndPlay, PostInitalizeComponents ->액터의 셋팅이 
- 게임플레이를 도와주는 프레임워크 시스템 사용 (멀티플레이, 장르...)
- <img src="Image/Mode_Set.png" height="300" title="Mode_Set">

   - 폰의 타입, 시작 레벨을 지정한다.  

- <img src="Image/Player_login.jpg" height="300" title=" Player_login">

   - GameMode, PlayerController, Pawn의 입장 순서를 나타낸다.

## **05.29**
> **<h3>Today Dev Story</h3>**

- ### 폰의 제작
   - <img src="Image/Create_Pawn.png" height="300" title="Create_Pawn"> 
   - Pawn에는 Capsule(충돌), SkeletalMesh(위치&애니메이션), FloatingPawnMovement(이동), SpringArm(카메라 구도), Camera(카메라)로 구성
   - Gamemode에 의해 생성, Camera는 Spring의 하위 컴포넌트
      <details><summary>코드 보기</summary>

      ```c++
      //ABPawn.h
      #include "GameFramework/FloatingPawnMovement.h"
      ...
      UPROPERTY(VisibleAnywhere, Category=Collision)
      UCapsuleComponent* Capsule;

      UPROPERTY(VisibleAnywhere, Category=Visual)
      USkeletalMeshComponent * Mesh;

      UPROPERTY(VisibleAnywhere, Category=Movement)
      UFloatingPawnMovement *Movement;

      UPROPERTY(VisibleAnywhere, Category=Camera)
      USpringArmComponent* SpringArm;

      UPROPERTY(VisibleAnywhere, Category=Camera)
      UCameraComponent* Camera;

      //ABPawn.cpp
      Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
      Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
      Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
      SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
      Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

      RootComponent = Capsule;
      Mesh->SetupAttachment(Capsule);
      SpringArm->SetupAttachment(Capsule);
      Camera->SetupAttachment(SpringArm);

      Capsule->SetCapsuleHalfHeight(88.0f);
      Capsule->SetCapsuleRadius(34.0f);
      Mesh->SetRelativeLocationAndRotation(FVector(0.0f,0.0f,-88.0f),FRotator(0.0f,-90.0f,0.0f));
      SpringArm->TargetArmLength = 400.0f;
      SpringArm->SetRelativeRotation(FRotator(-15.0f,0.0f,0.0f));

      static ConstructorHelpers::FObjectFinder<USkeletalMesh>
      SK_CARDBOARD(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard"));

      if(SK_CARDBOARD.Succeeded())
      {
         Mesh->SetSkeletalMesh(SK_CARDBOARD.Object);
      }
      ```

      </details>

- ### 폰의 조작
   - <img src="Image/Input_Set.png" height="300" title="Input_Set">
   - <img src="Image/Pawn_Move.gif" height="300" title="Pawn_Move">
   - Pawn의 이동을 위해 InputComponent라는 언리얼 컴포넌트 제공. 폰의 멤버 함수와 입력 설정을 연결하면 입력 신호는 자동으로 전달
   - 그것이 바로 SetupInputComponent 함수. BindAxis, BindAction을 제공
   - 위의 사진처럼 UpDown, LeftRight 매핑을 추가로 구현하고 AddMovementInput함수를 통해 이동
   - ABPlayerController의 BeginPlay()에 inputmode를 설정하여 자동으로 포커싱
   - #### Character와-동일한-구현
      <details><summary>코드 보기</summary>

      ```c++
      //ABPawn.h -> 함수 추가
      private:
      void UpDown(float NewAxisValue);
      void LeftRight(float NewAxisValue);

      //ABPawn.cpp
      void AABPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
      {
         Super::SetupPlayerInputComponent(PlayerInputComponent);

         PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABPawn::UpDown);          //설정한 키값을 누르면 UpDown을 실행
         PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABPawn::LeftRight);
      }

      void AABPawn::LeftRight(float NewAxisValue)  //움직임
      {
         AddMovementInput(GetActorForwardVector(), NewAxisValue); //GetActorForwardVector->월드 방향 기준으로 전진 방향
      }

      void AABPawn::UpDown(float NewAxisValue)
      {
         AddMovementInput(GetActorRightVector(), NewAxisValue);
      }

      //ABPlayerController.cpp -> 화면을 클릭하여 포커스를 잡지 않아도 자동으로 잡아줌
      void AABPlayerController::BeginPlay()
      {
         Super::BeginPlay();

         FInputModeGameOnly InputMode;
         SetInputMode(InputMode);
      }
      ```

      </details>
   
- ### 폰의 애니메이션 설정

   1. 코드로 애니메이션 구현
      - <img src="Image/Pawn_Animation.gif" height="300" title="Pawn_Animation">
      - 코드로 애니메이션을 지정할때 게임의 규모가 커지면 애니메이션 재생에 관리적인 한계에 부딪힌다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABPawn.cpp
         void AABPawn::BeginPlay()
         {
            Super::BeginPlay();

            //애니메이션을 호출
            Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
            UAnimationAsset *AnimAsset = LoadObject<UAnimationAsset>(nullptr, TEXT("/Game/Book/Animations/WarriorRun.WarriorRun"));

            if(AnimAsset != nullptr)
            {
               Mesh->PlayAnimation(AnimAsset, true);
            }
         }
         ```

         </details>

   2. 애니메이션 블루프린트로 애니메이션 구현
      - <img src="Image/Ani_BluePrint.png" height="300" title="Ani_BluePrint"><img src="Image/Animation_BluePrint.png" height="300" title="Animation_BluePrint">
      - 애니메이션은 코드 구현하기 복잡해서 애니메이션 블루프린트를 사용한다. (실행화면은 코드 구현과 동일)

         <details><summary>코드 보기</summary>

         ```c++
         //애니메이션을 호출
         Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
         static ConstructorHelpers::FClassFinder<UAnimInstance>
         WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
         
         if(WARRIOR_ANIM.Succeeded())
         {
            Mesh->SetAnimInstanceClass(WARRIOR_ANIM.Class);
         }
         ```

         </details>

- ### 캐릭터 모델
   - 인간형 Pawn이 아닌 효과적으로 제작하기 위한 Character를 생성 (Pawn을 상속받은 형태)
   - FloatingPawnMovement가 아닌 CharacterMovement를 사용하여 구현에 있어 더욱 효과적이다. ex) 멀티 동기화, 중력, 다양한 움직임
   - 코드는 Pawn과 동일하며 Mesh, capsule, movement는 기본제공하여 따로 구현할 필요가 없고 Get을 사용하여 호출한다. 아래와 같다.
   - ABGameMode에서 기본 Pawn으로 ABPawn을 사용하지 않고 ABCharacter로 변경해준다.
      ```c++
      //ABCharater.cpp
      GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
      //ABGameMode.ccp
      #include "ABCharacter.h"
      
      DefaultPawnClass = AABCharacter::StaticClass();
      ```
   - [Pawn의 코드](#Character와-동일한-구현)

- ### 컨트롤 회전의 활용
   - <img src="Image/Turn_Camera.gif" height="300" title="Turn_Camera">
   - Character에서 제어하며, 입력의 Turn(Z축), LookUp(Y축)축 설정을 가져와 사용한다. (-3 ~ 3)
   - AddControllerInputYaw, Roll, Pitch라는 3가지 명령어로 제공한다. 이때 카메라와 폰의 회전이 연동되어 폰의 Rotation 값도 변경된다.
      - Pawn섹션에 있는 UseControllRotationYaw를 사용하면 해제할 수 있다.
   - 틸드(~) 키를 사용하여 콘솔 창에 "displayall PlayerController ControlRotattion"을 입력하면 실시간으로 회전 값을 볼 수 있다.
      <details><summary>코드 보기</summary>

      ```c++
      //ABCharacter.cpp
      PlayerInputComponent->BindAxis(TEXT("Turn"),this,&AABCharacter::Turn);
	   PlayerInputComponent->BindAxis(TEXT("LookUp"),this,&AABCharacter::LookUp);
      ...
      ...
      void AABCharacter::Turn(float NewAxisValue)
      {
         AddControllerYawInput(NewAxisValue);
      }

      void AABCharacter::LookUp(float NewAxisValue)
      {
         AddControllerPitchInput(NewAxisValue);
      }
      ```

      </details>

> **<h3>Realization</h3>**

   - PlayerController에서는 인터페이스 기능도 다룬다.
      - 움직임을 컨트롤하지만 구현은 Pawn이나 Character에 되어있다.
   - 애니메이션은 코드가 아닌 블루프린트로 사용하는게 효과적
   - Pawn < Character 효과적 (FloatingPawnMovement가 아닌 CharacterMovement를 사용한다.)
      - 이는 멀티플레이시 자동으로 동기화를 지원하며 중력도 지원한다.

## **05.30**

> **<h3>Today Dev Story</h3>**

- ### 삼인칭 컨트롤 구현(GTA 방식)
   - <img src="Image/SpringArm_Camera.gif" height="300" title="SpringArm_Camera">
   - SpringArm 컴포넌트를 사용하여 편리하게 구현이 가능하다.
   - 이전 방식과는 다르게 시점을 이동해도 캐릭터가 이동하지 않는다. (bUseControllerRotationYaw값 설정)
   - 상하좌우로 시점이 변경되고, 카메라 사이에 장애물이 감지되면 캐릭터가 보이도록 한다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABCharacter.h
      void SetControlMode(int32 ControlMode); //protected
      
      //ABCharacter.cpp
      //스피링 암을 활요한 삼인칭 조작
      AABCharacter::AABCharacter()
      { 
         ...
	      SetControlMode(0);
      }

      void AABCharacter::SetControlMode(int32 ControlMode)
      {
         if(ControlMode == 0)
         {
            SpringArm->TargetArmLength = 450.0f;
            SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
            SpringArm->bUsePawnControlRotation = true;
            SpringArm->bInheritPitch = true;
            SpringArm->bInheritRoll = true;
            SpringArm->bInheritYaw = true;
            SpringArm->bDoCollisionTest = true;
            bUseControllerRotationYaw = false;
         }
      }
      ```

      </details>

   - <img src="Image/Camera_Follow_Pawn.gif" height="300" title="Camera_Follow_Pawn">
   - 카메라의 방향으로 직진할 수 있도록 구현. (Rotaion값으로 Vector데이터를 얻어 사용)
   - OrientRotationToMovement 기능을 사용하여 SetControlMode에서 캐릭터의 회전을 구현한다. (회전, 속도)
      <details><summary>코드 보기</summary>

      ```c++
      //ABCharacter.cpp
      void AABCharacter::UpDown(float NewAxisValue)
      {
         //AddMovementInput(GetActorForwardVector(), NewAxisValue); 이전방식
         AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X),NewAxisValue);
      }

      void AABCharacter::LeftRight(float NewAxisValue)
      {
         //AddMovementInput(GetActorRightVector(), NewAxisValue);    -> 카메라와 상관 없음
         AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y),NewAxisValue);	//카메라의 방향대로 사용
      }
      //캐릭터의 회전
      void AABCharacter::SetControlMode(int32 ControlMode)
      {
         if(ControlMode == 0)
         {
            ...
            GetCharacterMovement()->bOrientRotationToMovement = true;	//캐릭터 회전 여부
		      GetCharacterMovement()->RotationRate = FRotator(0.0f,720.0f,0.0f); //회전 속돟
         }
      }
      ```

      </details>

- ### 삼인칭 컨트롤 구현 (디아블로 방식)
   - 입력모드를 구분하도록 클래스 새로운 연거형을 선언, 현재 입력 모드를 보관할 멤버 변수를 추가했다. (각축의 입력을 보관한 벡터 추가)
      - <img src="Image/GTA_Type.gif" height="300" title="GTA_Type">
         
         - GTA방식 : 상하 키와 좌우 키를 각각처리
      - <img src="Image/DIABLO_Type.gif" height="300" title="DIABLO_Type">
         
         - 디아블로방식 : 상하좌우를 조합하여 회전과 이동을 처리
   - SetControlMode의 Switch로 인자 값을 분리해 구별하고 각 카메라와 플레이어 이동에서 변경한다.
   - 캐릭터의 자연스러운 전환을 위해서 CharcherMovement의 UseControllerDesiredRotation 속성을 체크한다. (Diablo만 GTA는 다른 방식)
      <details><summary>코드 보기</summary>

      ```c++
      //ABCharater.h
      enum class EControlMode { GTA, DIABLO };

      void SetControlMode(EControlMode NewControlMode);

      EControlMode CurrentControlMode = EControlMode::GTA;
      FVector DirectionToMove = FVector::ZeroVector;
      
      //ABCharacter.cpp
      void AABCharacter::Tick(float DeltaTime)
      {
         Super::Tick(DeltaTime);

         switch (CurrentControlMode)
         {
         case EControlMode::DIABLO:
            if(DirectionToMove.SizeSquared() > 0.0f)
            {
               GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
               AddMovementInput(DirectionToMove);
            }
            break;
         }
      }
      ...
      void AABCharacter::UpDown(float NewAxisValue)
      {
         switch (CurrentControlMode)
         {
         case EControlMode::GTA:
            AddMovementInput(FRotationMatrix(FRotator(0.0f,GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X),NewAxisValue);
            break;
         case EControlMode::DIABLO:
            DirectionToMove.X = NewAxisValue;
            break;
         }
      }  
      ...
      void AABCharacter::LookUp(float NewAxisValue)
      {
         switch (CurrentControlMode)
         {
         case EControlMode::GTA:
            AddControllerPitchInput(NewAxisValue);
            break;
         }
      }
      ...
      void AABCharacter::SetControlMode(EControlMode NewControlMode)
      {
         CurrentControlMode = NewControlMode;
         switch (CurrentControlMode)
         {
         case EControlMode::GTA:
            ...
            GetCharacterMovement()->bOrientRotationToMovement = true;	//캐릭터 회전 여부
            GetCharacterMovement()->bUseControllerDesiredRotation = false;	//끄면 부드럽게?
            GetCharacterMovement()->RotationRate = FRotator(0.0f,720.0f,0.0f); //회전 속도
            break;
         case EControlMode::DIABLO:
            ...
            GetCharacterMovement()->bOrientRotationToMovement = false;	//캐릭터 회전 여부
            GetCharacterMovement()->bUseControllerDesiredRotation = true;	//끄면 부드럽게?
            GetCharacterMovement()->RotationRate = FRotator(0.0f,720.0f,0.0f); //회전 속도
            break;
         }
      }
      ```

      </details>

- ### 컨트롤 설정의 변경
   - <img src="Image/View_Switch.gif" height="300" title="View_Switch">
   - 특정 키(Shift + V)를 입력해서 앞의 두가지 조작 방식을 전환할 수 있도록 구현
      - FMath 클래스에서 제공하는 InterpTo(일정한 속도로 지정 목표까지 이동)를 사용하여 자연스럽게 변경
   - 프로젝트 셋팅의 입력에서 ViewChange를 제작
   - SetControlMode에서 SpringArm의 길이와 회전을 Tick에서 변경하고 ViewChange에서 모드이름과 ControlRotation을 변환한다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABCharacter.h
      //자연스러운 이동을 위한 변수들
      protected:
         float ArmLengthTo = 0.0f;
         FRotator ArmRotationTo = FRotator::ZeroRotator;
         float ArmLengthSpeed = 0.0f;
         float ArmRotationSpeed = 0.0f;

      private:
         void ViewChange();

      //ABCharater.cpp
      AABCharacter::AABCharacter() {
         ...
      	ArmLengthSpeed = 3.0f;
         ArmRotationSpeed = 10.0f;
      }

      void AABCharacter::SetControlMode(EControlMode NewControlMode) {
	      CurrentControlMode = NewControlMode;
         switch (CurrentControlMode)
         {
         case EControlMode::GTA:
            ArmLengthTo = 450.0f;
            ...
         case EControlMode::DIABLO:
            ArmLengthTo = 800.0f;
            ArmRotationTo = FRotator(-45.0f,0.0f,0.0f);
         }
      }

      void AABCharacter::Tick(float DeltaTime)
      {
         Super::Tick(DeltaTime);

         SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

         switch (CurrentControlMode)
         {
         case EControlMode::DIABLO:
            SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetTargetRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed));
            if(DirectionToMove.SizeSquared() > 0.0f)
            {
               GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
               AddMovementInput(DirectionToMove);
            }
            break;
         }
      }

      void AABCharacter::ViewChange()
      {
         switch (CurrentControlMode)
         {
         case EControlMode::GTA:
            GetController()->SetControlRotation(GetActorRotation()); //카메라 회전
            SetControlMode(EControlMode::DIABLO);
            break;
         case EControlMode::DIABLO:
            GetController()->SetControlRotation(SpringArm->GetRelativeRotation());  
            SetControlMode(EControlMode::GTA);
            break;
         }
      }
      ```

      </details>

- ### 애니메이션 블루프린트
   - <img src="Image/Anim_Instance.gif" height="300" title="Anim_Instance">
   - c++로 애님 인스턴스를 제작하여 Pawn의 속력을 저장하고 이 값에 따라 애님 그래프에서 애니메이션을 구분.
   - ABAnimInstance.cpp 생성 후 CurrentPawnSpeed라는 float 변수 추가 후 참조 가능하도록 설정
   - <img src="Image/Set_BluePrint.gif" height="300" title="Set_BluePrint">
      - 블루프린트의 클래스 세팅>디테일>부모 클래스를 ABAnimInstance로 변경, 위 그림과 같이 구조 형성
   - 애님 인스턴스에서 Pawn의 속도 정보를 가져와 CurrentPawnSpeed에 업데이트한다.
   - 애님 인스턴스는 NativeUpdateAnimation 함수가 틱마다 호출된다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABAnimInstance.h
      public:
         UABAnimInstance();
         virtual void NativeUpdateAnimation(float DeltaSeconds) override;  //매 틱마다 불러온다.

      private:
         UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, Meta=(AllowPrivateAccess=true))
         float CurrentPawnSpeed; //속도
         
      //ABAnimInstance.cpp
      UABAnimInstance::UABAnimInstance()
      {
         CurrentPawnSpeed = 0.0f;
      }

      void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
      {
         Super::NativeUpdateAnimation(DeltaSeconds);

         auto Pawn = TryGetPawnOwner();	//유효검사
         if(::IsValid(Pawn))
         {
            CurrentPawnSpeed = Pawn->GetVelocity().Size();	//속도를 얻어온다.
         }
      }
      ```

      </details>

- ### 스테이트 머신 제작
   - <img src="Image/State_Machine.png" height="300" title="State_Machine">
   - 애님 그래프는 반복 수행을 설계하는 기능을 갖는 스테이트 머신 기능을 제공한다.
   - 위 사진과 같이 AnimGraph에 있던 애니메이션을 BaseAction의 Ground(스테이트)로 옮겼다.
   - 2번의 Entry 노드와 연결된 것을 시작 스테이트라고 하며, 이외의 연결은 조건을 통해 연결된다.

- ### 점프 구현_1
   - <img src="Image/Jump_Ani.gif" height="300" title="Jump_Ani">
   - ACharater 클래스에는 Jump라는 멤버 함수가 있으며, 바인딩이 가능하다.
   - 높이의 조절을 원하는 경우 GetCharacterMovement로 가져와서 JumpZVelcity 값을 변경한다. (※기본 420)
   - 점프의 경우 속도가 포함되기 때문에 달리기 모션이 재생된다. 이를 블루프린트에서 구현한다.
   - 폰의 점프 상황을 보관하기 위해 IsInAir라는 boolean타입을 선언하고 IsFalling함수를 호출해 동기화한다.
   - <img src="Image/Jump_Ani.png" height="300" title="Jump_Ani">
      
      - Ground <-> Jump의 트랜지션 조건을 위와 같이  설정한다.
      - 아직 Jump 내부는 구현하지 않았다.

      <details><summary>코드 보기</summary>

      ```c++
      //점프만.
      //ABCharacter.cpp
      GetCharacterMovement()->JumpZVelocity = 800.0f;
      ...
      void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
      {
         Super::SetupPlayerInputComponent(PlayerInputComponent);

         //내가 제작한 함수를 사용(뷰변경)
         PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
         //엔진에서 제공하는 함수를 사용(점프)
         PlayerInputComponent->BindAction(TEXT("Jump"),EInputEvent::IE_Pressed, this, &ACharacter::Jump);
         ...
      }
      //점프 애니메이션
      //ABAnimInstance.h
      UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, Meta=(AllowPrivateAccess=true))
	   bool IsInAir;

      //ABAnimInstance.cpp
      void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
      {
         Super::NativeUpdateAnimation(DeltaSeconds);

         auto Pawn = TryGetPawnOwner();	//유효검사
         if(::IsValid(Pawn))
         {
            CurrentPawnSpeed = Pawn->GetVelocity().Size();	//속도를 얻어온다.
            auto Character = Cast<ACharacter>(Pawn);  //캐릭터를 찾아
            if(Character)
            {
               IsInAir = Character->GetMovementComponent()->IsFalling();
            }
         }
      }
      ```

      </details>

- ### 애니메이션 리타겟
   - <img src="Image/Retarget.png" height="300" title="Retarget">
   - 내가 가지고 있는 캐릭터에는 점프 애니메이션이 존재하지 않기 다른 스켈레톤의 애니메이션을 가져와야한다.
   - 각 타겟(2가지)의 스켈레톤을 리타깃(매핑)하면 서로 애니메이션을 교환할 수 있다.

- ### 점프 구현_2
   - <img src="Image/Jump_Ani_2.gif" height="300" title="Jump_Ani_2">
   - 점프 동작은 지형에 따라 체공 시간이 달라 지형을 고려하여 "도약(1번), 체공(무한), 착지(1번)"로 나뉜다.
   - 애니메이션의 종료시 다른 애니메이션으로의 전환을 쉽게 하려면 트랜지션 노드에서 제공하는 'Automatic Rule Based on Sequen Player in State'를 체크하면된다.

- ### 애니메이션 몽타주 (공격 모션)
   - <img src="Image/Montage.gif" height="300" title="Montage">
   - 연속된 모션으로 공격하도록 애니메이션 기능 구현하기 위해 몽타주라는 기능을 사용한다.
   - 애님 몽타주 에셋을 제작하여 섹션을 단위로 애니메이션을 관리한다.
   - 위의 사진은 Attack1이라는 섹션으로 저장되며 세션 이름을 사용해 이를 재생할 수 있다.
   - <img src="Image/Attack_Log.gif" height="300" title="Attack_Log">
   - 이전과 동일한 방식으로 키의 입력을 받아 로그를 띄우고 ABCharacter에서 ABAnimInstance.h를 추가해 PlayAttackMontage를 실행한다.
   - 몽타주을 계속 체크하여 폰에게 알려주는 방식

      <details><summary>코드 보기</summary>

      ```c++
      //ABAnimInstance.h
      public:
      	void PlayAttackMontage();
      private:
      	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category=Attack, Meta=(AllowPrivateAccess=true))
         UAnimMontage* AttackMontage;
      //ABAnimInstance.cpp
      static ConstructorHelpers::FObjectFinder<UAnimMontage>
      ATTACK_MONTAGE(TEXT("/Game/Book/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage"));
      
      if(ATTACK_MONTAGE.Succeeded())
      {
         AttackMontage = ATTACK_MONTAGE.Object;
      }

      void UABAnimInstance::PlayAttackMontage()
      {
         if(!Montage_IsPlaying(AttackMontage))
         {
            Montage_Play(AttackMontage, 1.0f);
         }
      }
      //ABCharacter.cpp
      #include "ABAnimInstance.h" //헤더 추가 후
      void AABCharacter::Attack()
      {
         ABLOG_S(Warning);

         auto AnimInstance = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
         if(nullptr == AnimInstance) return;

         AnimInstance->PlayAttackMontage();
      }
      ```

      </details>

- ### 델리게이트 
   - 몽타주의 재생이 끝나면 재공격 가능 여부를 알려주는 방식이 더 효과적이다. 이 방식이 바로 델리게이트이다.
   - 특정 객체가 해야 할 로직을 다른 객체가 처리할 수 있도록 하는 설계이다. 하지만 c++에서는 지원하지 않기에 별도로 구축한 프레임워크를 사용한다.
      - AnimInstance에서는 몽타주가 끝나면 OnMontageEnded 델리게이트를 제공한다.


> **<h3>Realization</h3>**

- 애니메이션 블루프린트는 아래와 같은 시스템이 있다.
   1. 애님 인스턴스 : Pawn의 정보를 받아 애님 그래프가 참조할 데이터를 제공. c++과 블루프린트로 제작 가능
   2. 애님 테이블 : 변수 값에 따라 변화하는 애니메이션 시스템을 설계. 블루프린트로만 제작가능
- 틱마다 입력 시스템 -> 게임 로직 -> 애니메이션 시스템순으로 실행한다.
- 현재의 움직임을 파악하기 위해서 4가지 함수가 사용된다. (FloatingPawnMovement에서는 false를 반환)
   - IsFalling(), IsSwimming(), IsCrouching(), IsMoveOnGround()이다.
- 스테이트 머신을 사용하여 애니메이션을 구현하는데 계속 추가한다면 복잡하기에 몽타주라는 기능을 사용한다.
- 델리게이트를 c++에서는 지원하지 않아 엔진에서 제공하는 별도의 델리게이트 프레임워크를 사용해야한다. 
   - AnimInstance의 OnMontageEnded라는 델리게이트이다.

|기술|특징|방식|
|:--:|:--:|:--:|
|몽타주|특정 상황에서 애니메이션 발동하며 스테이트를 추가하지 않아도 된다. |몽타주를 계속 체크하는 방식|
|델리게이트|특정 객체가 해야할 것을 다른 객체가 처리한다. |몽타주가 종료되면 체크|



## **05.31**

> **<h3>Today Dev Story</h3>**
   - null

> **<h3>Realization</h3>**
   - null
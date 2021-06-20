# ArenaBattle
- 이득우의 언리얼 c++ 게임 개발의 정석을 읽으면서 만들어본 3D게임

## **05.28**

> **<h3>Today Dev Story</h3>**
- ### <span style = "color:yellow;">고정된 분수대의 구현</span>
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

- ### <span style = "color:yellow;">움직이는 액터 설계</span>
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

- ### <span style = "color:yellow;">로그의 출력 방법</span>
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


- ### <span style = "color:yellow;">GameMode & Pawn & PlayerController 설정</span>
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

- ### <span style = "color:yellow;">플레이어의 입장</span>
   - <img src="Image/Player_Login.png" height="300" title="Player_Login"> 
   - 플레이어가 로그인(입장)하면 언리얼 엔진에서 PostLogin 이벤트 함수가 호출된다. 
      - 이 과정에서 플레이어가 조종한 폰을 생성하고 플레이어 컨트롤러가 해당 폰에 빙의하는 작업이 이루어진다.
   - 각 생성되는 시점은 PostInitializeComponents, 빙의를 진행하는 시점은 PalyerController의 Possess, Pawn의 PossesedBy로 알 수 있다.
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

- ### <span style = "color:yellow;">폰의 제작</span>
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

- ### <span style = "color:yellow;">폰의 조작</span>
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
   
- ### <span style = "color:yellow;">폰의 애니메이션 설정</span>

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

- ### <span style = "color:yellow;">캐릭터 모델</span>
   - 인간형 Pawn이 아닌 효과적으로 제작하기 위한 Character를 생성 (Pawn을 상속받은 형태)
   - FloatingPawnMovement가 아닌 CharacterMovement를 사용하여 구현에 있어 더욱 효과적이다. ex) 멀티 동기화, 중력, 다양한 움직임
   - 코드는 Pawn과 동일하며 Mesh, capsule, movement는 기본제공하여 따로 구현할 필요가 없고 Get을 사용하여 호출한다. 아래와 같다.
   - ABGameMode에서 기본 Pawn으로 ABPawn을 사용하지 않고 ABCharacter로 변경해준다.
      ```c++
      //ABCharacter.cpp
      GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
      //ABGameMode.ccp
      #include "ABCharacter.h"
      
      DefaultPawnClass = AABCharacter::StaticClass();
      ```
   - [Pawn의 코드](#Character와-동일한-구현)

- ### <span style = "color:yellow;">컨트롤 회전의 활용</span>
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

- ### <span style = "color:yellow;">삼인칭 컨트롤 구현(GTA 방식)</span>
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

- ### <span style = "color:yellow;">삼인칭 컨트롤 구현 (디아블로 방식)</span>
   - 입력모드를 구분하도록 클래스 새로운 연거형을 선언, 현재 입력 모드를 보관할 멤버 변수를 추가했다. (각축의 입력을 보관한 벡터 추가)
      - <img src="Image/GTA_Type.gif" height="300" title="GTA_Type">
         
         - GTA방식 : 상하 키와 좌우 키를 각각처리
      - <img src="Image/DIABLO_Type.gif" height="300" title="DIABLO_Type">
         
         - 디아블로방식 : 상하좌우를 조합하여 회전과 이동을 처리
   - SetControlMode의 Switch로 인자 값을 분리해 구별하고 각 카메라와 플레이어 이동에서 변경한다.
   - 캐릭터의 자연스러운 전환을 위해서 CharcherMovement의 UseControllerDesiredRotation 속성을 체크한다. (Diablo만 GTA는 다른 방식)
      <details><summary>코드 보기</summary>

      ```c++
      //ABCharacter.h
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

- ### <span style = "color:yellow;">컨트롤 설정의 변경</span>
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

      //ABCharacter.cpp
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

- ### <span style = "color:yellow;">애니메이션 블루프린트</span>
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

- ### <span style = "color:yellow;">스테이트 머신 제작</span>
   - <img src="Image/State_Machine.png" height="300" title="State_Machine">
   - 애님 그래프는 반복 수행을 설계하는 기능을 갖는 스테이트 머신 기능을 제공한다.
   - 위 사진과 같이 AnimGraph에 있던 애니메이션을 BaseAction의 Ground(스테이트)로 옮겼다.
   - 2번의 Entry 노드와 연결된 것을 시작 스테이트라고 하며, 이외의 연결은 조건을 통해 연결된다.

- ### <span style = "color:yellow;">점프 구현_1</span>
   - <img src="Image/Jump_Ani.gif" height="300" title="Jump_Ani">
   - ACharacter 클래스에는 Jump라는 멤버 함수가 있으며, 바인딩이 가능하다.
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

- ### <span style = "color:yellow;">애니메이션 리타겟</span>
   - <img src="Image/Retarget.png" height="300" title="Retarget">
   - 내가 가지고 있는 캐릭터에는 점프 애니메이션이 존재하지 않기 다른 스켈레톤의 애니메이션을 가져와야한다.
   - 각 타겟(2가지)의 스켈레톤을 리타깃(매핑)하면 서로 애니메이션을 교환할 수 있다.

- ### <span style = "color:yellow;">점프 구현_2</span>
   - <img src="Image/Jump_Ani_2.gif" height="300" title="Jump_Ani_2">
   - 점프 동작은 지형에 따라 체공 시간이 달라 지형을 고려하여 "도약(1번), 체공(무한), 착지(1번)"로 나뉜다.
   - 애니메이션의 종료시 다른 애니메이션으로의 전환을 쉽게 하려면 트랜지션 노드에서 제공하는 'Automatic Rule Based on Sequen Player in State'를 체크하면된다.

- ### <span style = "color:yellow;">애니메이션 몽타주 (공격 모션)</span>
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

- ### <span style = "color:yellow;">델리게이트 </span>
   - 몽타주의 재생이 끝나면 재공격 가능 여부를 알려주는 방식이 더 효과적이다. 이 방식이 바로 델리게이트이다.
   - 특정 객체가 해야 할 로직을 다른 객체가 처리할 수 있도록 하는 설계이다. 하지만 c++에서는 지원하지 않기에 별도로 구축한 프레임워크를 사용한다.
      - AnimInstance에서는 몽타주가 끝나면 OnMontageEnded 델리게이트를 제공한다. 이를 통해 몽타주가 끝나는 타이밍 파악 가능. UFUNCTION 매크로추가
   - 애님 인스턴스에 몽타주의 끝을 알리고 원하는 함수를 실행시킨다. (이전 몽타주의 PlayAttackMontage의 조건(if)은 삭제한다.)
   - 추가로 애님 인스턴스를 자주 사용하기 때문에 ABAnim이라는 클래스를 연동해 두었다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABCharacter.h
      virtual void PostInitializeComponents() override;
       ...
      UFUNCTION()
      void OnAttackMontageEnded(UAnimMontage *Montage, bool bInterrupted);

      private:
         UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Attack, Meta = (AllowPrivateAccess = true))
         bool IsAttacking;
         
         //자주 사용한 애님 인스턴스
         UPROPERTY()
         class UABAnimInstance* ABAnim;

      //ABCharacter.cpp
      IsAttacking = false;
      ...
      void AABCharacter::PostInitializeComponents()
      {
         Super::PostInitializeComponents();
         ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
         ABCHECK(nullptr != ABAnim);

         //행동이 끝나면 다른 함수에게 알려준다.
         ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded);
      }
      ...
      void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
      {
         ABCHECK(IsAttacking);   //새로만든 매크로
         IsAttacking = false;
      }
      ```

      </details>

- ### <span style = "color:yellow;">애니메이션 노티파이</span>
   - <img src="Image/Notify_Ani.gif" height="300" title="Notify_Ani">
   - 애니메이션을 재생하는 동안 특정 타이밍에 애님 인스턴스에게 신호를 보내는 기능이다. (일반 애니메이션, 몽타주 가능)
   - 몽타주 시스템에서 원하는 타이밍에 노티파이를 추가하고 노티파이가 호출되면 "AnimNotify_노티파이명"을 찾아 호출한다.
      - 언리얼 런타임이 찾을 수 있도록 UNFUNCTION()을 추가해준다.
   - <img src="Image/Notify.png" height="300" title="Notify">
      
      - 위 사진처럼 노티파이와 섹션을 추가해 구분하고 연결 세션을 해제한다. (※ 이때 노티파이가 반을 넘어가면 오류가 발생한다.)
      - 이때 AttackHitCheck, NextAttackCheck가 존재하며 각각 공격모션완료, 다음공격여부의 체크이다. 
   - 코드의 원리
      - ABCharacter에서 콤보에 사용할 MaxCombo, CurrentCombo, CanNextCombo, IsComboInputOn를 변수로 선언한다.
      - 공격의 시작과 종료를 지정할 AttackStart(End)ComboState 함수를 선언한다.
      - ABAnimInstance에서 콤보 카운트를 전달받으면 해당 몽타주 섹션을 재생하고 NextAttackCheck 노티파이 발생시 ABCharacter에 전달할 델리게이트틑 선언하고 호출한다.
         - 이 델리게이트 기능을 자신이 뭐하는지 몰라도 연결된 함수만 있어도 한다. (※멀티캐스트로 선언했다.)
      - 공격을 하면 ABCharacter는 콤보가 가능한지 파악하고 NextAttackCheck 타이밍 전까지 명령어가 오면 다음 콤보를 시작한다.
      
      <details><summary>코드 보기</summary>
      
      ```c++
      //ABAnimInstance.h
      DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
      DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);
      ...
      public:
      UABAnimInstance();
      virtual void NativeUpdateAnimation(float DeltaSeconds) override;

      void PlayAttackMontage();
      void JumpToAttackMontageSection(int32 NewSection);

      public:
         FOnNextAttackCheckDelegate OnNextAttackCheck;
         FOnAttackHitCheckDelegate OnAttackHitCheck;

      private:
         UFUNCTION()
         void AnimNotify_AttackHitCheck();

         UFUNCTION()
         void AnimNotify_NextAttackCheck();

         FName GetAttackMontageSectionName(int32 Section);

      private:
         UFUNCTION()
         void AnimNotify_AttackHitCheck();
         
      //ABAniInstance.cpp
      void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
      {
         ABCHECK(Montage_IsPlaying(AttackMontage));
         Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
      }

      void UABAnimInstance::AnimNotify_AttackHitCheck()
      {
         OnAttackHitCheck.Broadcast();
      }

      void UABAnimInstance::AnimNotify_NextAttackCheck()
      {
         OnNextAttackCheck.Broadcast();
      }

      FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
      {
         ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
         return FName(*FString::Printf(TEXT("Attack%d"), Section));
      }

      //ABCharacter.h	
      void AttackStartComboState();
      void AttackEndComboState();
      ...

      UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
      bool CanNextCombo;

      UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
      bool IsComboInputOn;

      UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
      int32 CurrentCombo;

      UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
      int32 MaxCombo;

      //ABCharacter.
      void AABCharacter::PostInitializeComponents()
      {
         ...

         //콤보
         ABAnim->OnNextAttackCheck.AddLambda([this]() -> void
         {
            ABLOG(Warning, TEXT("OnNextAttackCheck"));
            CanNextCombo = false;

            if(IsComboInputOn)
            {
               AttackStartComboState();
               ABAnim->JumpToAttackMontageSection(CurrentCombo);
            }
         });
      }
      void AABCharacter::Attack()
      {
         if(IsAttacking)
         {
            ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo,1,MaxCombo));
            if(CanNextCombo)
            {
               IsComboInputOn = true;
            }
         }
         else
         {
            ABCHECK(CurrentCombo == 0);
            AttackStartComboState();
            ABAnim->PlayAttackMontage();
            ABAnim->JumpToAttackMontageSection(CurrentCombo);
            IsAttacking = true;
         }
      }

      void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
      {
         ABCHECK(IsAttacking);
         ABCHECK(CurrentCombo > 0);
         IsAttacking = false;
         AttackEndComboState();
      }
      void AABCharacter::AttackStartComboState()	//콤보 시작
      {
         CanNextCombo = true;
         IsComboInputOn = false;
         ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
         CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
      }
      void AABCharacter::AttackEndComboState()	//콤보 종료
      {
         IsComboInputOn = false;
         CanNextCombo = false;
         CurrentCombo = 0;
      }
      ```
      
      </details>
      
- ### <span style = "color:yellow;">콜리전 & 트레이스 채널 설정</span>
   - <img src="Image/Attack_Collision.gif" height="300" title="Attack_Collision">
   - 완성후 로그를 띄운화면
      1. ### 콜리전 설정
         - <img src="Image/Collision_Set.png" height="300" title="Collision_Set">
         - 언리얼 엔진에서는 무시 반응을 최대화하고 블록 반응을 최소화한다.
         - 위의 사진과 같이 "ABCharacter"라는 이름으로 Object Channel과 Preset을 만들어 준 뒤 설정을 변경하고 ABCharacter를 할당해준다.
            ```c++
            //ABCharacter.cpp
            //내가 만든 콜리전을 할당
            GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));
            ```
      
      2. ### 트레스 채널 설정
         - <img src="Image/Channel_Num.png" height="300" title="Channel_Num">
         - 위의 사진은 할당된 Channel 번호이다.
         - 공격 행동은 특정 타이밍에 발생되기 때문에  <span style = "color:yellow;">트레이스 채널</span>을 사용해야한다.
         - Attack 트레이스 채널을 생성하고 ABCharacter 콜리전과의 반응을 블록으로 지정한다.
         - 트레이스 채널을 사용해 물리적 충돌 여부를 가리는 함수 SweepSingleByCannel을 사용하여 공격 판정을 내리는 로직을 추가한다.
            - 기본 도형을 인자로 받아 시작 지점에서 끝 지점까지 쓸면서 물리 판정이 일어났는지를 확인한다.
            - FCollisionShape::MakeShpere을 통해 도형을 제작하고 탐색하고 충돌된 액터를 구조체(FHitResult)로 넘긴다.
         
            <details><summary>코드 보기</summary>

            ```c++
            //ABCharacter.cpp
            AABCharacter::AABCharacter()
            {
               ....
               //내가 만든 콜리전을 할당
               GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));
            }
            //가상의 구를 만들어 공격을 체크 
            void AABCharacter::AttackCheck()
            {
               FHitResult HitResult; //맞은 정보를 저장
               FCollisionQueryParams Params(NAME_None, false, this);
               bool bReslut = GetWorld()->SweepSingleByChannel(
                  HitResult,
                  GetActorLocation(),
                  GetActorLocation() + GetActorForwardVector() * 200.0f,
                  FQuat::Identity,
                  ECollisionChannel::ECC_GameTraceChannel2,	//Attack의 채널번호
                  FCollisionShape::MakeSphere(50.0f),
                  Params);

               if(bReslut)
               {
                  if(HitResult.Actor.IsValid())
                  {
                     ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());
                  }
               }
            }
            ```

            </details>

> **<h3>Realization</h3>**
   - ### 람다식 구문
      - 헤더에 선언할 필요없이 간단한 로직 구현
      - 람다 소개자 : []로 표시되며 람다 구문이 참조할 환경을 지정
      - 파라미터 리스트 : 람다 함수가 사용할 파라미터를 지정 (※ 빈괄호 가능)
      - 후행 반환 타입 : 람다 함수가 반환할 타입을 지정
      - 람다 함수 구문 : {}에 로직을 넣어준다.
         ```c++
         //example
         [this]() -> void{ ~~~ };
         ```

   - ### 콜리전
      |방법|설명|
      |:--:|:--:|
      |스태틱메시 에셋|스태틱메시 에셋에 콜리전 영역을 심는 방법이다. 에디터에서 확인 가능하며 해당 컴포넌트에서 비주얼과 충돌 두가지 기능을 설정 가능하다. BlockAll이라는 기본 설정이 있어 캐릭터의 이동을 방해하는 콘텐츠를 제작 가능하다.|
      |기본도형 컴포넌트|기본 도형을 사용해 충돌영역을 지정하는 방법이다. 별도의 충돌영역을 제작할때 사용하며 스켈레탈 메시를 움직일때 사용한다.|
      |피직스 애셋|헝겊 인형효과를 구현할때 사용하며, 스켈레탈 메시에만 사용할 수 있다.|

## **06.01**

> **<h3>Today Dev Story</h3>**
- ### <span style = "color:yellow;">디버그 드로잉</span>
   - <img src="Image/Debug_Drawing.gif" height="300" title="Debug_Drawing">
   - 공격할때 디보그 드로잉 기능을 사용해서 공격 범위를 시각적으로 보이게 한다. 닿지 않으면 빨간색 닿으면 녹색으로 표시된다.
   - 이를 위해서는 소스 상단에 DrawDebugHelper.h를 추가해야한다.
      <details><summary>코드 보기</summary>

      ```c++
      //ABCharacter.h
      //디버그 드로잉
      UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Attack, Meta=(AllowPrivateAccess = true))
      float AttackRange;

      UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Attack, Meta=(AllowPrivateAccess = true))
      float AttackRadius;

      //ABCharacter.cpp
      #include "DrawDebugHelpers.h"
      ...
      AABCharacter::AABCharacter()
      {
         ...
         AttackRange = 200.0f;
         AttackRadius = 50.0f;
      }
      ...
      void AABCharacter::AttackCheck()
      {
         ...
      #if ENABLE_DRAW_DEBUG
         //설정들
         FVector TraceVec = GetActorForwardVector() * AttackRange;
         FVector Center = GetActorLocation() + TraceVec * 0.5f;
         float HalfHeight = AttackRange * 0.5f + AttackRadius;
         FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
         FColor DrawColor = bReslut ? FColor::Green : FColor::Red;
         float DebugLifeTime = 5.0f;

         //실제로 그려지는 부분
         DrawDebugCapsule(GetWorld(),Center,HalfHeight,AttackRadius,CapsuleRot,DrawColor,false,DebugLifeTime);

      #endif

         if(bReslut)
         {
            if(HitResult.Actor.IsValid())
            {
               ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());
            }
         }
      }
      ```

      </details>

- ### <span style = "color:yellow;">데미지 프레임워크</span>
   - <img src="Image/Damage_FrameWork.gif" height="300" title="Damage_FrameWork">
   - 엔진이 제공하는 데이미 프레임워크를 사용하여 액터에 데미지를 쉽게 전달할 수 있다.
      - AActor에 TakeDamage라는 함수가 구현되어 있다. 이 함수를 사용하여 쉽게 정달가능하다. (※모든 액터에는 Can be Damaged 속성이 있다.)
   - 파라미터 중 가해자 인자가 있는데 이것은 Pawn이 아닌 Controller의 정보를 보내줘야한다.  
   - AttackCheck()에서 데미지를 전달하고 50이상의 데미지가 들어왔다면 죽는 모션을 보여주고 Collision을 끈다. (더 이상 실행 X) 

      <details><summary>코드 보기(데미지 판단)</summary>

      ```c++
      //ABCharacter.h
      public:	
	      virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
      
      //ABCharacter.cpp
      float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
      {
         float FinalDamage = Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator,DamageCauser);
         ABLOG(Warning, TEXT("Actor : %s took Damage : %f"),*GetName(),FinalDamage);
         return FinalDamage;
      }

      void AABCharacter::AttackCheck()
      {
         ...
         ...
         if(bReslut)
         {
            if(HitResult.Actor.IsValid())
            {
               ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

               //데미지의 전달
               FDamageEvent DamageEnvent;
               HitResult.Actor->TakeDamage(50.0f, DamageEnvent, GetController(), this);
            }
         }
      }
      ```

      </details>


      <details><summary>코드 보기(죽는 애니메이션)</summary>

      ```c++
      //ABAnimInstance.h
      void SetDeadAnim() { IsDead = true;}
      ...
      UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, Meta=(AllowPrivateAccess=true))
	   bool IsDead;
      //ABAnimInstance.cpp ->  여러가지 추가
      //ABCharacter.cpp -> 애니메이션 추가
      float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
      {
         ...
         if(FinalDamage > 0.0f)
         {
            ABAnim->SetDeadAnim();
            SetActorEnableCollision(false);
         }
         return FinalDamage;
      }
      ```

      </details>


> **<h3>Realization</h3>**
- 애니메이션 에디터에 접근시 비정상적으로 종료되는 현상이 발생되었는데, 패키지를 모두 설치하지 않아 해결에 어려움이 있었다.
- 단순히 c++내의 문맥 오류였는데 애니메이션 접근시 오류가 발생해 오랜 시간이 소요되었다.   

## **06.02**
> **<h3>Today Dev Story</h3>**
- ### <span style = "color:yellow;">무기 착용</span>
   - <img src="Image/Weapon.gif" height="300" title="Weapon">
   - <img src="Image/Socket_Setting.gif" height="300" title="Socket_Setting">
   - 무기와 같은 액세서리는 트랜스폼이 아닌 메시에 착용해야 한다. 그를 위해 소켓이라는 시스템을 지원한다.
   - 스켈레톤 트리 > hand_rSocket > BlackKnight 무기 착용 후 위치 값을 조정한다.
   - 무기는 바뀔수 있기 때문에 따로 Actor 클래스의 ABPeapon을 제작하였다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABWeapon.h
      UPROPERTY(VisibleAnywhere, Category=Weapon)
	   USkeletalMeshComponent* Weapon;
      //ABWeapon.cpp
      AABWeapon::AABWeapon()
      {
         // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
         PrimaryActorTick.bCanEverTick = false; //
         
         //칼을 넣어줌
         Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
         RootComponent = Weapon;

         static ConstructorHelpers::FObjectFinder<USkeletalMesh>
         SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blade/Axes/Blade_AnthraciteAxe/SK_Blade_AnthraciteAxe.SK_Blade_AnthraciteAxe"));
         if(SK_WEAPON.Succeeded())
         {
            Weapon->SetSkeletalMesh(SK_WEAPON.Object);
         }
         Weapon->SetCollisionProfileName(TEXT("NoCollision"));	//충돌 없음
      }

      //ABCharacter.cpp
      #include "ABWeapon.h"
      void AABCharacter::BeginPlay()
      {
         Super::BeginPlay();
         
         FName WeaponSocket(TEXT("hand_rSocket"));
         auto CurWeapon = GetWorld()->SpawnActor<AABWeapon>(FVector::ZeroVector, FRotator::ZeroRotator);
         if(nullptr != CurWeapon)
         {
            CurWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
         }
      }
      ```

      </details>

- ### <span style = "color:yellow;">아이템 상자의 제작</span>
   - <img src="Image/WeaponBox.png" height="300" title="WeaponBox">
   - 플레이어에게 무기를 공급해줄 아이템 상자를 제작한다. (Actor를 부모로 하는 ABItemBox생성)
   - 플레이어를 감지하는 콜리전 박스(Root), 아이템 상자를 시각화해주는 스태틱메시(자식)로 나뉜다.

      <details><summary>코드 보기</summary>

      ```c++
      AABItemBox::AABItemBox()
      {
         // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
         PrimaryActorTick.bCanEverTick = false;

         Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
         Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));

         RootComponent = Trigger;
         Box->SetupAttachment(RootComponent);

         Trigger->SetBoxExtent(FVector(40.0f,42.0f,30.0f));	//박스의 절반값
         static ConstructorHelpers::FObjectFinder<UStaticMesh>
         SM_BOX(TEXT("/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1"));
         if(SM_BOX.Succeeded())
         {
            Box->SetStaticMesh(SM_BOX.Object);
         }
         Box->SetRelativeLocation(FVector(0.0f,-3.5f,-30.0f));
      }
      ```

      </details>

   - <img src="Image/Overlap.png" height="300" title="Overlap">
   - 폰이 아이템을 획득하도록 아이템 상자에 오브젝트 채널을 추가한다. ItemBox라는 오브젝트 채널과(Ignore) 프로그파일을 생성하고 플레이어에 대해서만 겹칩으로 설정한다.
   - 박스 컴포넌트에서는 캐릭터를 감지할 수 있게 Overlap 이벤트를 처리하도록 OnComponentBeginOverlap이라는 델리게이트를 사용한다.
      
      <details><summary>코드 보기</summary>

      ```c++
      //ABItemBox.h
      protected:
         virtual void PostInitializeComponents() override;
      private:
         UFUNCTION()
         void OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor *OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,boolbFromSweep, const FHitResult& SweepResult);
      
      //ABItemBox.cpp
      AABItemBox::AABItemBox()
      {
         //충돌 이름
         Trigger->SetCollisionProfileName(TEXT("ItemBox"));
         Box->SetCollisionProfileName(TEXT("NoCollision"));
      }
      ...
      void AABItemBox::PostInitializeComponents()
      {
         Super::PostInitializeComponents();
         Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnCharacterOverlap);	//닿으면 실행한다.
      }

      void AABItemBox::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
      {
         ABLOG_S(Warning);
      }
      ```

      </details>

> **<h3>Realization</h3>**
   - 무기와 같은 액세서리는 트랜스폼이 아닌 메시에 착용해야 한다. 그를 위해 소켓이라는 시스템을 지원한다.


## **06.03**
> **<h3>Today Dev Story</h3>**
- ### <span style = "color:yellow;">아이템의 습득</span>
   1. ### 아이템 상자를 통과하면 빈손의 플레이어에게 무기를 쥐어주는 기능을 구현하며, 클래스 정보를 저장할 속성을 추가하고 이 값을 기반으로 생성한다.
   - <img src="Image/Equip_Weapon.gif" height="350" title="Equip_Weapon">

      - 클래스 정보 저장시 UClass의 포인트가 아닌 TSubclassof라는 키워드를 사용해 특정 상속 클래스만을 보이게한다.
      - 무기를 장착시키는 SetWeapon이라는 함수를 선언하여 현재 무기가 없으면 장착하고 소유자를 캐릭터로 변경한다. (기존 BeginPlay내의 로직은 삭제)
      - 상자에 다가가면 무기가 착용되고 두 번 습득하면 착용할 수 없다는 로그가 발생한다. (기존 구현한 Overlap함수 사용)

         <details><summary>코드 보기</summary>

         ```c++
         //ABItemBox.h
         UPROPERTY(EditInstanceOnly, Category=Box)
         TSubclassOf<class AABWeapon> WeaponItemClass;
         
         //ABItemBox.cpp
         #include <ABCharacter.h>
         AABItemBox::AABItemBox()
         {
            ...
            //해당 속성에 대한 기본 클래스 값을 지정한다.
            WeaponItemClass = AABWeapon::StaticClass();
         }
         ...
         void AABItemBox::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) //오버랩이 진행되었을때
         {
            ABLOG_S(Warning);

            auto ABCharacter = Cast<AABCharacter>(OtherActor);
            ABCHECK(nullptr != ABCharacter);

            if(nullptr != ABCharacter && nullptr != WeaponItemClass)
            {
               if(ABCharacter->CanSetWeapon())
               {
                  auto NewWeapon = GetWorld()->SpawnActor<AABWeapon>(WeaponItemClass,FVector::ZeroVector, FRotator::ZeroRotator);
                  ABCharacter->SetWeapon(NewWeapon);
               }
               else
               {
                  ABLOG(Warning,TEXT("%s can't equip weapon currently"),*ABCharacter->GetName());
               }
            }
         }

         //ABCharacter.h
         //무기 생성
         bool CanSetWeapon();
         void SetWeapon(class AABWeapon* NewWeapon);

         UPROPERTY(VisibleAnywhere, Category=Weapon)
         class AABWeapon* CurrentWeapon;

         //ABCharacter.cpp
         bool AABCharacter::CanSetWeapon()	//무기 장착 가능 여부
         {
            return (nullptr == CurrentWeapon);
         }

         void AABCharacter::SetWeapon(AABWeapon* NewWeapon)	//무기를 장착시키는 로직
         {
            ABCHECK(nullptr != NewWeapon && nullptr == CurrentWeapon);
            FName WeaponSocket(TEXT("hand_rSocket"));
            if(nullptr != NewWeapon)
            {
               NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
               NewWeapon->SetOwner(this);
               CurrentWeapon = NewWeapon;
            }
         }
         ```

         </details>

   2. ### 아이템을 습득하면 이펙트를 재생하고 상자가 사라지는 기능을 구현한다.
   - <img src="Image/Box_Effect.gif" height="300" title="Box_Effect">

      - 상자 액터에 파티클 컴포넌트를 추가한 후 경로를 지정한다. 멤버 변수를 추가하여 파티클 컴포넌트에서 제공하는 OnSystemFinished 델리데이트에 연결해 로직을 구현한다. (UFUNCTION 매크로 선언)
      - 이펙트 재생 기간동안은 액터 충돌 기능, 박스 스테딕메시를 제거해 무기를 두번 습득하지 못하도록 방지한다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABItemBox.h
         UPROPERTY(VisibleAnywhere, Category=Effect)
         UParticleSystemComponent* Effect;
         ...
         UFUNCTION()
         void OnEffectFinished(class UParticleSystemComponent* PSystem);

         //ABItemBox.cpp
         AABItemBox::AABItemBox()
         {
            Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));
            Effect->SetupAttachment(RootComponent);
            
            static ConstructorHelpers::FObjectFinder<UParticleSystem>
            P_CHESTOPEN(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh"));
            if(P_CHESTOPEN.Succeeded())
            {
               Effect->SetTemplate(P_CHESTOPEN.Object);
               Effect->bAutoActivate = false;	
            }
         }
         ...
         void AABItemBox::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
         {
            ...
            if(nullptr != ABCharacter && nullptr != WeaponItemClass)
            {
               if(ABCharacter->CanSetWeapon())
               {
                  ...
                  Effect->Activate(true);	//효과 발생
                  Box->SetHiddenInGame(true, true);	//숨기는 기능
                  SetActorEnableCollision(false);	//collsion기능 끔
                  Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished);	//끝나면
               }
               ...
            }
         }
         ...
         void AABItemBox::OnEffectFinished(UParticleSystemComponent* PSystem)
         {
            Destroy();
         }
         ```

         </details>

   3. ### 아이템 상자에서 다른 무기를 생성할 수 있도록 추가한다.
   - <img src="Image/Other_Weapon.gif" height="300" title="Other_Weapon">

      - 매번 c++ 클래스를 추가하는 것은 번거로우니 블루프린트를 사용해 ABWeapon을 상속받은 객체를 다수 생성한다.
      - 상자의 WeaponClass를 방금 생성한 클래스로 수정하면 완료된다.

- ### <span style = "color:yellow;">엑셀 데이터 활용</span>
   - 엑셀에 저장되어 있는 캐릭터의 스탯 데이터 테이블을 엔진에 불러오는 기능을 구현한다.
   - 게임 인스턴스를 통해 캐릭터의 스탯을 관리하도록 설정하면 초기화시 데이터를 불러들이고 종료시까지 보존할 수 있다.
   - GameInstance를 부모로 하는 클래스 ABGameInstance를 제작하고 프로젝트 세팅 > 맵 & 모드의 항목을 변경한다.
      - 게임 앱이 초기화되면 엔진은 GameInstance의 Init함수를 가장 먼저 구현한다.

   1. ### 데이터 애셋 임포트
   - <img src="Image/CSV_Import.png" height="300" title="CSV_Import">

      - CSV파일을 불러오기 위해서는 각 열의 이름과 유형이 동일한 구조체를 선언해야한다. 
         - FTableRowBase > FABCharacterData라는 구조체 헤더에 선언
         - USTRUCT, GENERATED_BODY 매크로 선언
      - CSV파일(ABCharacterDate.csv)을 임포트 할때 데이터 테이블 행 타입을 ABCharacterData로 바꾸고 적용한다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABGameInstance.h
         #include "Engine/DataTable.h"

         USTRUCT(BlueprintType)
         struct FABCharacterData : public FTableRowBase
         {
            GENERATED_BODY()

         public:
            FABCharacterData() : Level(1), MaxHP(100.0f), Attack(10.0f), DropExp(10), NextExp(30){}

            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
            int32 Level;
            
            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
            float MaxHP;
            
            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
            float Attack;
            
            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
            int32 DropExp;
            
            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data")
            int32 NextExp;
         };

         //ABGameInstance.cpp
         UABGameInstance::UABGameInstance()
         {
            
         }

         void UABGameInstance::Init()
         {
            Super::Init();
            ABLOG_S(Warning);
         }

         ```

         </details>

   2. ### 데이터 에셋을 게임 인스턴스에서 로딩하는 기능
   - <img src="Image/Check_TableData.png" height="200" title="Check_TableData">

      - 테이블 데이터를 관리하기 위해 DataTable이라는 오브젝트를 제공한다.
      - Init 함수에서 해당 데이터가 잘 로딩되었는지 파악하도록 20Level의 데이터를 출력한다. 
          
         <details><summary>코드 보기</summary>

         ```c++
         //ABGameInstance.h
         FABCharacterData* GetABCharacterData(int32 Level);

         private:
            UPROPERTY()
            class UDataTable* ABCharacterTable;
         
         //ABGameInstance.cpp
         UABGameInstance::UABGameInstance()
         {
            FString CharacterDataPath = TEXT("/Game/Book/GameData/ABCharacterData.ABCharacterData");
            static  ConstructorHelpers::FObjectFinder<UDataTable>
            DT_ABCHARACTER(*CharacterDataPath);

            ABCHECK(DT_ABCHARACTER.Succeeded());
            ABCharacterTable = DT_ABCHARACTER.Object;
            ABCHECK(ABCharacterTable->GetRowMap().Num() > 0);
         }
         ...
         void UABGameInstance::Init()
         {
            Super::Init();
            ABLOG(Warning, TEXT("DropExp of Level 20 ABCharacter : %d"),GetABCharacterData(20)->DropExp);
         }

         FABCharacterData* UABGameInstance::GetABCharacterData(int32 Level)
         {
            return ABCharacterTable->FindRow<FABCharacterData>(*FString::FromInt(Level),TEXT(""));
         }
         ```

         </details>

> **<h3>Realization</h3>**
- <img src="Image/Playing_Process.jpg" height="200" title="Playing_Process">

   - 게임시작 과정은 아래 사진과 같다.
- 엑셀로 저장된 데이터를 불러오기 위해서는 CSV파일로 저장되어 있어야한다.


## **06.04**
> **<h3>Today Dev Story</h3>**
- ### <span style = "color:yellow;">액터 컴포넌트 제작</span>
   1. ### 액터 컴포넌트 클래스(ABCharacterStatComponent)를 생성하고 이를 ABCharacter에 부착해 스텟에 대한 관리를 추가한다.
      - <img src="Image/CharacterStat.png" height="300" title="CharacterStat"> <img src="Image/Stat_Level.gif" height="300" title="Stat_Level">

      - Tick로직이 필요없다. PostInitialzeComponenets 이전에 호출되는 InitializeComponent를 통하여 초기화 로직을 구현한다. (bWantsInitializeComponent 값을 True로 설정해주어야한다.) 
      - 모든 스탯을 스탯 컴포넌트에서 관리하도록 변수를 모두 private으로 선언하고 레벨은 SetNewLevel을 통해서 변경할 수 있도록한다.
      - 위의 사진을 보면 Level에 따라 CurrentHP값이 변동하는 것을 볼 수 있다.
         
         <details><summary>코드 보기</summary>

         ```c++
         //ABCharacter.h
         UPROPERTY(VisibleAnywhere, Category=Stat)
         class UABCharacterStatComponent *CharacterStat;

         //ABCharacter.cpp
         #include "ABCharacterStatComponent.h"
         ...
         CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));

         //ABCharacterStatComponent.h
         protected:
            virtual void InitializeComponent() override;

         public:	
            void SetNewLevel(int32 NewLevel);
         
         private:
            struct FABCharacterData* CurrentStatData = nullptr;
         
            UPROPERTY(EditInstanceOnly, Category=Stat, Meta=(AllowPrivateAccess = true))
            int32 Level;
         
            UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat, Meta=(AllowPrivateAccess = true))	//직렬화에서 제외 (게임 시작시마다 변경)
            float CurrentHP;
         };

         //ABCharacterStatComponent.cpp
         UABCharacterStatComponent::UABCharacterStatComponent()
         {
            PrimaryComponentTick.bCanEverTick = false;
            bWantsInitializeComponent = true;

            Level = 1;	//1로 초기화
         }
         ...
         void UABCharacterStatComponent::InitializeComponent()
         {
            Super::InitializeComponent();
            SetNewLevel(Level);	//초기화
         }

         void UABCharacterStatComponent::SetNewLevel(int32 NewLevel)
         {
            auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

            ABCHECK(nullptr != ABGameInstance);
            CurrentStatData = ABGameInstance->GetABCharacterData(NewLevel);
            if(nullptr != CurrentStatData)
            {
               Level = NewLevel;
               CurrentHP = CurrentStatData->MaxHP;
            }
            else
            {
               ABLOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
            }
         }
         ```

         </details>

   2. ### 캐릭터가 대미지를 받으면 대미지 만큼 CurrentHP에서 차감하고 그 결과로 CurrentHP 값이 0보다 작으면 죽도록 기능을 추가한다.
      - <img src="Image/Damage_Decrease.gif" height="300" title="Damage_Decrease">

      - TakeDamage 함수에서 직접 처리했지만 ABCharacterStatComponent에 SetDamage함수를 생성하고 TakeDamage 함수에서 호출 후 처리한다.
         - 액터 컴포넌트가 캐릭터에 의존성을 가지지 않도록 액터 컴포넌트에 델리게이트를 선언하고 바인딩하는 구조로 설계한다.
         - 기존 ABCharacter에 존재하던 애니메이션 구조를 델리게이트와 연동하도록 바꾼다.
      
         <details><summary>코드 보기</summary>

         ```c++
         //ABCharacterStatComponent.h
         DECLARE_MULTICAST_DELEGATE(FOnHPIsZeroDelegate);
         ...
         public:	
            void SetDamage(float NewDamage);
            float GetAttack();
         
         FOnHPIsZeroDelegate OnHpIsZero;
         
         //ABCharacterStatComponent.cpp
         void UABCharacterStatComponent::SetDamage(float NewDamage)	//데미지를 받는다.
         {
            ABCHECK(nullptr != CurrentStatData);
            CurrentHP = FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP); //사이값이여야한다.
            if(CurrentHP <= 0.0f)
            {
               OnHpIsZero.Broadcast();	//방송때려라
            }
         }
         float UABCharacterStatComponent::GetAttack()	//데미지를 준다.
         {
            ABCHECK(nullptr != CurrentStatData, 0.0f);
            return CurrentStatData->Attack;
         }
         
         //ABCaracter.cpp
         void AABCharacter::PostInitializeComponents()
         {
            ...
            CharacterStat->OnHpIsZero.AddLambda([this]() -> void
            {
               ABLOG(Warning, TEXT("OnHpIsZero"));
               ABAnim->SetDeadAnim();
               SetActorEnableCollision(false);
            });
         }
         ...
         float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
         {
            ....
            CharacterStat->SetDamage(FinalDamage);
            return FinalDamage;
         }
         ...
         void AABCharacter::AttackCheck()
         {
            ...
            if(bReslut)
            {
               if(HitResult.Actor.IsValid())
               {
                  ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

                  FDamageEvent DamageEnvent;
                  HitResult.Actor->TakeDamage(CharacterStat->GetAttack(), DamageEnvent, GetController(), this);
               }
            }
         }
         ```

         </details>
   
- ### 캐릭터 위젯 UI제작
   - <img src="Image/UI_HPBar.png" height="300" title="UI_HPBar">
   - HP값이 시각적으로 보이도록 UI위젯을 제작후 캐릭터에 부착한다. -> UI폴더를 만들고 위젯 블루프린트(UI_HPBar)를 생성한다.
      - 작게 프로그레스바를 생성할 것임으로 Fill Screen이 아닌 Custom(150*50)으로 바꾼다. 또한 같은 이유로 Canvas를 제거하고 일반/Progress Bar를 지어준다.
      - PB_HPBar라고 이름을 지어준후 Vertical Box로 감싸준다.
      - 프리미티브/Spacer 컨트롤을 첫 번째와 세 번째에 추가하고 size를 40씩 채워준다.

- ### 모듈과 빌드 설정
   - <img src="Image/HP_UI.png" height="300" title="HP_UI">
   - UI를 캐릭터에 부착하기 위해 UWidgetComponent라는 클래스를 제공한다.
   - 초기 그냥 컴파일을 하면 "알 수 없는 외부참조"라는 오류가 발생하는데 현재 프로젝트에 UI관련 모듈을 지정하지 않았기 때문이다.
      - <img src="Image/Buildcs.png" height="200" title="Buildcs">
      - Arena.Build.cs에서 확인할 수 있으며, 위 사진과 같이 "UMG"를 추가해주어야 사용할 수 있다.
   - UMG 모듈의 Public/Components 폴더에는 현재 사용중인 WidgetComponent.h 파일이 있는데 이 파일을 추가해 컴포넌트를 생성하는 코드를 생성한다.
   - 위젯은 캐릭터 머리 위로 오도록 하고 위젯 블루프린트 애셋의 레퍼런스를 사용해 클래스 정보를 컴포넌트의 WidgetClass로 등록한다.
   - 또한 플레이어를 향해 보도록 Screen모드로 지정하고 크기는 150 * 50으로 지정한다.
      
      <details><summary>코드 보기</summary>

      ```c++
      //ABCharacter.h
      UPROPERTY(VisibleAnywhere, Category=UI)
	   class UWidgetComponent* HPBarWidget;

      //ABCharacter.cpp
      #include "Components/WidgetComponent.h"
      ...
      AABCharacter::AABCharacter()
      {
         ...
      	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
         ...
      	HPBarWidget->SetupAttachment(GetMesh());
         ...
         HPBarWidget->SetRelativeLocation(FVector(0.0f,0.0f,180.0f));
         HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);	//스크린 모드
         
         static ConstructorHelpers::FClassFinder<UUserWidget>
         UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
         if(UI_HUD.Succeeded())
         {
            HPBarWidget->SetWidgetClass(UI_HUD.Class);
            HPBarWidget->SetDrawSize(FVector2D(150.0f,50.0f));
         }
      }
      ```

      </details>

- ### UI와 데이터 연동
   - <img src="Image/HPBar_End.gif" height="300" title="HPBar_End">
   - UI의 로직은 c++클래스(UserWidget)에서 미리 만들어 제공할 수 있다. 이를 상속받는 ABCharacterWidget을 생성한다.
   - 해당 클래스는 정보가 저장되어 있는 ABCharacterStatComponent와 연동해 스텟이 변동할때마다 HP바를 업데이트한다. (델리게이트 사용)
   - 앞의 위젯 블루프린트를 ABCharacterWidget을 상속받도록한다.
   
      <details><summary>코드 보기(ABCharacterStatComponent)</summary>

      ```c++
      //ABCharacterStatComponent.h
      DECLARE_MULTICAST_DELEGATE(FOnHpChangedDelegate);
      public:	
         ...
         void SetHP(float NewHP);
         ...
         float GetHPRatio();
         ...
         FOnHpChangedDelegate OnHpChanged;
      //ABCharacterStatComponent.cpp
      void UABCharacterStatComponent::SetNewLevel(int32 NewLevel)
      {
         ...
         if(nullptr != CurrentStatData)
         {
            Level = NewLevel;
            SetHP(CurrentStatData->MaxHP);
         }
         ...
      }

      void UABCharacterStatComponent::SetDamage(float NewDamage)	//데미지를 받는다.
      {
         ABCHECK(nullptr != CurrentStatData);
         SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP));
      }

      void UABCharacterStatComponent::SetHP(float NewHP)
      {
         CurrentHP = NewHP;
         OnHpChanged.Broadcast();
         if(CurrentHP < KINDA_SMALL_NUMBER)
         {
            CurrentHP = 0.0f;
            OnHpIsZero.Broadcast();
         }
      }

      float UABCharacterStatComponent::GetHPRatio()	//체력 비율을 리턴
      {
         ABCHECK(nullptr != CurrentStatData, 0.0f);
         return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / CurrentStatData->MaxHP);
      }
      ```

      </details>

   - <img src="Image/Ratio_HP.png" height="200" title="Ratio_HP">
   -  UI에서 캐릭터 컴포넌트에 연결하는 과정. 약 포인터(TWeakObjectPtr)를 사용해 참조하였다. (공부용)
      - UI와 캐릭터가 다른 액터라면 약 포인터를 사용할 것
   - 연결 후 코드에서 PB_HPBar라는 이름의 위젯을 검색하고 속성을 업데이트하는 로직을 구현한다.
      - 이때 UI가 초기화되는 시점을 고려해야하는데 UI시스템은 준비되면 NativeConstruct 함수가 호출된다. 여기에 로직을 구현한다.

      <details><summary>코드 보기(ABCharacterWidget)</summary>

      ```c++
      //ABCharacterWidget.h
      public:
         void BindCharacterStat(class UABCharacterStatComponent* NewCharacterStat);

      protected:
         virtual void NativeConstruct() override;
         void UpdateHPWidget();

      private:
         TWeakObjectPtr<class UABCharacterStatComponent> CurrentCharacterStat;

         UPROPERTY()
         class UProgressBar* HPProgressBar;

      //ABCharacterWidget.cpp
      #include "Components/ProgressBar.h"

      void UABCharacterWidget::BindCharacterStat(UABCharacterStatComponent* NewCharacterStat)
      {
         ABCHECK(nullptr != NewCharacterStat);

         CurrentCharacterStat = NewCharacterStat;
         
         NewCharacterStat->OnHpChanged.AddUObject(this, &UABCharacterWidget::UpdateHPWidget);
      }

      void UABCharacterWidget::NativeConstruct()
      {
         Super::NativeConstruct();
         HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
         ABCHECK(nullptr != HPProgressBar);
         UpdateHPWidget();
      }

      void UABCharacterWidget::UpdateHPWidget()	//HP가 드러나는 부분
      {
         if(CurrentCharacterStat.IsValid())
         {
            if(nullptr != HPProgressBar)
            {
               HPProgressBar->SetPercent(CurrentCharacterStat->GetHPRatio());
            }
         }
      }
      //ABCharacter.cpp
      
      #include "ABCharacterWidget.h"
      void AABCharacter::BeginPlay()
      {
         Super::BeginPlay();
         
         auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
         if(nullptr != CharacterWidget)
         {
            CharacterWidget->BindCharacterStat(CharacterStat);
         }
      }
      ```

      </details>

- ### AIController와 내비게이션 시스템
   - <img src="Image/AI_Move.gif" height="300" title="AI_Move">
   - 비헤이비어 트리모델을 사용해 인공지능을 설계하고, 플레이어가 아닌 인공지능에 의해 스스로 정찰하고 플레이어를 쫓아와 공격하는 NPC를 제작한다.
   - 인공지능(AI)로 NPC를 제어하도록 AI컨트롤러를 제공한다. (폰도 빙의가 가능하다.)
   - 레벨에 배치된 캐릭터가 스스로 움직일 수 있도록 AI컨트롤러를 생성해 캐릭터에게 부여해본다. (AIController를 부모로 하는 ABAIController클래스 생성)
      - ABCharacter에 ABAIController.h를 추가. AI 생성 옵션을 PlaceInWorldOrSpawned로 설정 -> 플레이어를 죄외한 모든 캐릭터에 빙의

      <details><summary>코드 보기</summary>

      ```c++
      #include "ABAIController.h"
      AABCharacter::AABCharacter()
      {
         ...
      	AIControllerClass = AABAIController::StaticClass();
         AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
      }
      ```

      </details>

   - <img src="Image/NavMesh_Bounds_Volume.png" height="300" title="NavMesh_Bounds_Volume">
   - NPC는 스스로 움직여야하기 때문에 '내비게이션 메시'기반인 길 찾기 시스템을 사용해야한다.
      - 레벨에 내비게이션 메시를 배치해 NPC가 스스로 움직일 수 있는 환경을 구축해야 한다.
   - 모드/볼륨의 탭에서 Navmesh Bounds Volume을 배치하고 Brush Settings의 크기를 (10000 X 10000 X 500cm)으로 설정한다.
   - 이 내비메시영역을 활용해 ABAIController에 빙의한 폰에게 목적지를 알려줘 스스로 움직이도록 명령을 추가한다.
      - AI컨트롤에 타이머를 설치해 3초마다 폰에게 목적지로 이동하는 명령을 내린다.
      - 목적지를 랜덤으로 가져오는 함수 GetRandomPointInNavigableRadius, 목적지로 폰을 이동시키는 SimpleMoveToLocation함수를 제공한다.
      - ArenaBattle.Build.cs의 AddRange에 "NavigationSystem"를 추가해준다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABAIController.h
      public:
         AABAIController();
         virtual void OnPossess(APawn* InPawn) override;
         virtual void OnUnPossess() override;

      private:
         void OnRepeatTimer();

         FTimerHandle RepeatTimeHandle;
         float RepeatInterval;

      //ABAIController.cpp
      #include "ABAIController.h"
      #include "NavigationSystem.h"
      #include "Blueprint/AIBlueprintHelperLibrary.h"

      AABAIController::AABAIController()
      {
         RepeatInterval = 3.0f;
      }

      void AABAIController::OnPossess(APawn* InPawn)
      {
         Super::OnPossess(InPawn);
         GetWorld()->GetTimerManager().SetTimer(RepeatTimeHandle, this, &AABAIController::OnRepeatTimer, RepeatInterval, true);
      }

      void AABAIController::OnUnPossess()
      {
         Super::OnUnPossess();
         GetWorld()->GetTimerManager().ClearTimer(RepeatTimeHandle);
      }

      void AABAIController::OnRepeatTimer()
      {
         auto CurrentPawn = GetPawn();
         ABCHECK(nullptr != CurrentPawn);

         UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
         if(nullptr == NavSystem) return;

         FNavLocation NextLocation;
         if(NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.0f, NextLocation))
         {
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, NextLocation.Location);
            ABLOG(Warning, TEXT("Next Location : %s"), *NextLocation.Location.ToString());
         }
      }
      ```

      </details>

> **<h3>Realization</h3>**
- 언리얼 오브젝트에는 직렬화 기능이 있어서 UPROERTY 속성을 저장하고 로딩할 수 있다.
   - UPROERTY(Transient)는 직렬화를 사용하지 않는다는 것

- float와 값을 비교할때 미세한 오차 범위 내에 있는지를 판단하는 것이 좋은데 무시 가능한 오차를 측정할때 KINDA_SMALL_NUMBER를 사용한다.
- 4.21버전부터 위젯의 초기화 시점이 BeginPlay로 바뀌었다.
- 비헤이비어 트리(Behavior Tree)모델을 사용하여 인공지능을 설계할 수 있다.
- ArenaBattle.Build.cs의 AddRange에 "NavigationSystem"를 추가해주어야 네비게이션을 사용할 수 있다.
- ArenaBattle.Build.cs의 AddRange에 "CMG"를 추가해주어야 UI블루프린트를 사용할 수 있다.

## **06.05**
> **<h3>Today Dev Story</h3>**
- ### 비헤이비터 트리 시스템
   1. Behaviour 트리를 통한 로직 구현
      - <img src="Image/Behaviour.JPG" height="300" title="Behaviour">
      - NPC가 해야할 행동을 분석하고 우선순위가 높은 행동부터 NPC가 실행할 수 있도록 트리 구조로 성계하는 기법이다. (블랙보드 애셋이 필요하다.)
      - 비헤이비어 트리에 Task/Wait(일정기간 동안 대기) 태스크를 하나 생성한다.
         - 태스크들이 False 결과가 나올 때까지 좌우로 태스크를 실행하는 시퀀스 컴포짓을 선택해 추가한다.
         - ArenaBattle.Build.cs의 AddRange에 "AIModule"를 추가해주어야 비헤이비어 트리를 사용할 수 있다.
      - 기존 ABAIController 클래스의 모든 코드를 비헤이비어 트리 구동에 맞게 수정한다.
         - OnUnPossess(), OnRepeatTimer(), RepeatTimeHandle, RepeatInterval, NavigationSystem.h 삭제

         <details><summary>코드 보기</summary>

         ```c++
         //ABAIContorller.h
         public:
            AABAIController();
            virtual void OnPossess(APawn* InPawn) override;

         private:
            UPROPERTY()
            class UBehaviorTree* BTAsset;

            UPROPERTY()
            class UBlackboardData* BBAsset;
         
         //ABAIContorller.cpp
         #include "ABAIController.h"
         #include "BehaviorTree/BehaviorTree.h"
         #include "BehaviorTree/BlackboardData.h"

         AABAIController::AABAIController()
         {
            static ConstructorHelpers::FObjectFinder<UBlackboardData>
            BBObject(TEXT("/Game/Book/AI/BB_ABCharacter.BB_ABCharacter"));
            if(BBObject.Succeeded())
            {
               BBAsset = BBObject.Object;
            }

            static ConstructorHelpers::FObjectFinder<UBehaviorTree>
            BTObject(TEXT("/Game/Book/AI/BT_ABCharacter.BT_ABCharacter"));
            if(BTObject.Succeeded())
            {
               BTAsset = BTObject.Object;
            }
         }

         void AABAIController::OnPossess(APawn* InPawn)
         {
            Super::OnPossess(InPawn);
            if(UseBlackboard(BBAsset,Blackboard))
            {
               if(!RunBehaviorTree(BTAsset))
               {
                  ABLOG(Error,TEXT("AIController couldn't run behavior tree!"));
               }
            }
         }
         ```

         </details>

   2. Behaviour 트리를 통한 NPC 임의위치 순찰기능 구현
      - <img src="Image/Auto_Move.gif" height="300" title="Auto_Move">
      - 블랙보드에 순찰기능에 필요한 초기 NPC 생성위치를 저장 정보와 순찰한 위치 정보를 보관할 Vector 타입의 키를 선언 (HomePos,PartolPos) 
      - 트리 구동 전에 AIController에서 HomePos키 값을 지정 하도록 구현. FName 속성을 추가하고 할당
      - 순찰 PatrolPos 데이터는 순찰 할때마다 바뀌므로 태스크를 제작해 설계한다. (BTTaskMode를 상속받는 BTTask_FindPatrolPos)
         - ArenaBattle.Build.cs의 AddRange에 "GamePlayTasks"를 추가해주어야 비헤이비어 트리 에디터를 사용할 수 있다.(?)
         - Aborted : 중단, Failed : 실패, Succeeded : 성공, InProgress : 수행중 값들 중 하나를 반환한다.
      - Wait 태스크 오른쪽에 FindPatrolPos(방금 만든 c++ 파일)를 배치하고 MoveTo 태스크를 추가해 배치한다.
            
         <details><summary>코드 보기</summary>

         ```c++
         //ABAIContorller.h
         public:
            static const FName HomePosKey;
            static const FName PatrolPosKey;
         
         //ABAIContorller.cpp
         #include "BehaviorTree/BlackboardComponent.h"
         const FName AABAIController::HomePosKey(TEXT("HomePos"));
         const FName AABAIController::PatrolPosKey(TEXT("PatrolPos"));
         ...
         void AABAIController::OnPossess(APawn* InPawn)
         {
            Super::OnPossess(InPawn);
            if(UseBlackboard(BBAsset,Blackboard))
            {
               Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());	//Location을 HomePosKey에..
               if(!RunBehaviorTree(BTAsset))
               {
                  ABLOG(Error,TEXT("AIController couldn't run behavior tree!"));
               }
            }
         }
         //BTTask_FindPatrolPos.h
         public:
            UBTTask_FindPatrolPos();
            virtual  EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

         //BTTask_FindPatrolPos.cpp
         #include "BTTask_FindPatrolPos.h"
         #include "ABAIController.h"
         #include "NavigationSystem.h"
         #include "BehaviorTree/BlackboardComponent.h"
         #include "NavigationSystem.h"

         UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
         {
            NodeName = TEXT("FindPatrolPos");
         }

         EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
         {
            EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

            auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
            if(nullptr == ControllingPawn)
               return EBTNodeResult::Failed;

            UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
            if(nullptr == NavSystem)
               return EBTNodeResult::Failed;

            FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AABAIController::HomePosKey);
            FNavLocation NextPatrol;

            if(NavSystem->GetRandomPointInNavigableRadius(Origin, 500.0f,NextPatrol))
            {
               OwnerComp.GetBlackboardComponent()->SetValueAsVector(AABAIController::PatrolPosKey,NextPatrol.Location);
               return EBTNodeResult::Succeeded;
            }

            return EBTNodeResult::Failed;
            
         }
         ```

         </details>

- ### NPC의 추격 기능
   
   1. 정찰 중 플레이어가 1초마다 일정 반경(6미터) 내의 캐릭터를 탐지
      - <img src="Image/Detect_Service.gif" height="300" title="Detect_Service">
      - 플레이어를 발견할때 플레이어의 정보를 블랙보드에 저장하도록 Object 타입으로 Target 변수를 생성한다. (Object 기반은 ABCharacter로 지정)
      - NPC의 행동 패턴은 플레이어의 발견여부에 구분되므로 셀렉터 컴포짓을 통해 로직을 확장한다. 정찰 < 추격(Target을 향해 이동하도록 설계)
      - 이를 위해 서비스 노드를 제공하는데 독립적으로 동작하지 않고 컴포짓 노드에 부착되는 노드이다. 또한 해당 컴포짓에 속한 태스크들이 실행되는 동안 반복적인 작업을 실행하는데 적합하다. 
         - BTService를 부모로 하는 BTService_Detect 클래스를 생성, 서비스 노드가 속한 컴포짓 노드가 활성화되는 경우 TickNode함수를 호출한다. (주기는 Interval속성으로 지정)

         <details><summary>코드 보기</summary>

         ```c++
         //BTService_Detect.h
         public:
            UBTService_Detect();

         protected:
            virtual  void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

         //BTService_Detect.cpp
         #include "BTService_Detect.h"
         #include "ABAIController.h"
         #include "ABCharacter.h"
         #include "BehaviorTree/BlackboardComponent.h"
         #include "DrawDebugHelpers.h"

         UBTService_Detect::UBTService_Detect()
         {
            NodeName = TEXT("Detect");
            Interval = 1.0f; //주기
         }

         void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
         {
            Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

            APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
            if(nullptr == ControllingPawn) return;

            UWorld* World = ControllingPawn->GetWorld();
            FVector Center = ControllingPawn->GetActorLocation();
            float DetectRadius = 600.0f;	//범위

            if(nullptr == World) return;
            TArray<FOverlapResult> OverlapResults;
            FCollisionQueryParams CollisionQueryParams(NAME_None,false,ControllingPawn);

            //반경내의 모든 캐릭터를 탐지한다.
            bool bResult = World->OverlapMultiByChannel(
               OverlapResults,
               Center,
               FQuat::Identity,
               ECollisionChannel::ECC_GameTraceChannel2,
               FCollisionShape::MakeSphere(DetectRadius),
               CollisionQueryParams
            );

            DrawDebugSphere(World,Center,DetectRadius, 16, FColor::Red,false,0.2f);
         }
         ```

         </details>

   2. 감지되는 대상 중 주인공 캐릭터를 추려내는 기능과 추격 기능 추가
      - <img src="Image/AI_Follow.gif" height="300" title="AI_Follow">
      - 캐릭터를 조정하는 컨트롤러가 플레이어 컨트롤러인지 파악하도록 IsPlayerController 함수 사용
      - 탐지되면 녹색 구체를 그리고, 연결된 선을 그린다. 또한 Target 값을 플레이어로 지정하고 그렇지 않다면 nullptr로 지정한다. 
      - <img src="Image/Queck_Follow.png" height="300" title="Queck_Follow">
         
         - 탐색시 바로 시퀀스를 종료하고 추격하는 시퀀스를 하도록 설정 (Target, NoTarget) -> Target 값의 여부에 따라
         
         <details><summary>코드 보기</summary>

         ```c++
         //ABAIController.h
         static const FName TargetKey;
         //ABAIController.cpp
         const FName AABAIController::TargetKey(TEXT("Target"));
         //BTService_Detect.cpp
         //탐지되면 선을 그린다
         void UBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
         {
            ...
            if(bResult)
            {
               for (auto OverlapResult : OverlapResults)
               {
                  AABCharacter* ABCharacter = Cast<AABCharacter>(OverlapResult.GetActor());
                  if(ABCharacter && ABCharacter->GetController()->IsPlayerController()) 
                  {
                     OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, ABCharacter);
                     DrawDebugSphere(World, Center, DetectRadius,16, FColor::Green, false, 0.2f);
                     
                     DrawDebugPoint(World, ABCharacter->GetActorLocation(),10.0f, FColor::Blue, false, 0.2f);
                     DrawDebugLine(World, ControllingPawn->GetActorLocation(),ABCharacter->GetActorLocation(), FColor::Blue,false, 0.2f);
                     return;
                  }
               }
            }
            else
            {
               OwnerComp.GetBlackboardComponent()->SetValueAsObject(AABAIController::TargetKey, nullptr);
            }
            
            DrawDebugSphere(World,Center,DetectRadius, 16, FColor::Red,false,0.2f);
         }
         ```

         </details>

   3. NPC의 자연스러운 회전과 최대 이동 속도 수정
      - <img src="Image/Finish_AI.gif" height="300" title="Finish_AI">
      - NPC를 위한 ControlMode를 추가하고 이동방향에 따라 회전하도록 무브먼트 설정을 변경한다.
      - PossessedBy에서 플레이어가 컨트롤하는 건지 여부에 따라서 이동 속도 또한 낮게 설정해 도망갈 수 있도록 구현한다. 

         <details><summary>코드 보기</summary>

         ```c++
         //ABCharacter.h
         enum class EControlMode { GTA, DIABLO, NPC };
         ...
         virtual void PossessedBy(AController* NewController) override;
         
         //ABCharacter.cpp
         void AABCharacter::SetControlMode(EControlMode NewControlMode)
         {
            CurrentControlMode = NewControlMode;
            switch (CurrentControlMode)
            {
               case EControlMode::NPC:
            bUseControllerRotationYaw = false;
            GetCharacterMovement()->bUseControllerDesiredRotation = false;
            GetCharacterMovement()->bOrientRotationToMovement = true;
            GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
            break;
            }
            ...
         }
         ...
         void AABCharacter::PossessedBy(AController* NewController)  //속도 조정
         {
            Super::PossessedBy(NewController);

            if(IsPlayerControlled())
            {
               SetControlMode(EControlMode::DIABLO);
               GetCharacterMovement()->MaxWalkSpeed = 600.0f;
            }
            else
            {
               SetControlMode(EControlMode::NPC);
               GetCharacterMovement()->MaxWalkSpeed = 300.0f;
            }
         }
         ```

         </details>

> **<h3>Realization</h3>**
- 비헤이비어 트리 : 블랙보드 데이터에 기반해 설계한 비헤이비어 트리의 정보가 저장 (시각화)
   - 태스크 : 명령// 독립적으로 실행되지 않고 컴포짓(Composite)노드를 거쳐 실행된다.
   - 컴포짓 노드에는 셀렉터와 시퀀스가 있다. //시퀀스는 실패할때까지 계속 실행한다.
- 블랙보드 : 인공지능의 판단에 사용하는 데이터 집합
- ArenaBattle.Build.cs의 AddRange에 "AIModule"를 추가해주어야 비헤이비어 트리를 사용할 수 있다.
- ArenaBattle.Build.cs의 AddRange에 "GamePlayTasks"를 추가해주어야 비헤이비어 트리 에디터를 사용할 수 있다.(?)
   - 테스크를 실행할때 ExecuteTask라는 함수를 실행하는 넷 중하나의 값을 반환해야한다. 이 결과에 따라 다음 태스크를 결정한다.
      - Aborted : 중단, Failed : 실패, Succeeded : 성공, InProgress : 수행중
- 서비스 노드가 속한 컴포짓 노드가 활성화되는 경우 TickNode함수를 호출한다. (주기는 Interval속성으로 지정)
- 앞에 BT~_로 시작하는 C++는 비헤이비어 트리에서 자동으로 호출가능하다.

## **06.06**
> **<h3>Today Dev Story</h3>**
- ### NPC의 공격
   1. NPC의 추격후 공격 대신 1.5초간 정지
      - <img src="Image/IsInAttackRange.gif" height="300" title="IsInAttackRange">
      - 거리에 따라 추격이나 공격으로 분기하기에 기존 비헤이비어 트리의 왼쪽 로직을 두 갈래로 확장한다.
      - 블랙보드의 값을 사용하지 않고 목표물인 플레이어가 공격 범위 내에 있는지 데코레이터를 생성한다. 
         - BTDecorator를 상속받는 BTDecorator_IsInAttackRange를 생성하며 CalculateRawConditionValue 함수를 상속받아 조건 달성여부를 파악한다.
         - 비헤이비어 트리를 위 그림과 같이 수정한다.

         <details><summary>코드 보기</summary>

         ```c++
         //BTDecorator_IsInAttackRange.h
         public:
            UBTDecorator_IsInAttackRange();

         protected:
            virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
         
         //BTDecorator_IsInAttackRange.cpp
         #include "BTDecorator_IsInAttackRange.h"
         #include "ABAIController.h"
         #include "ABCharacter.h"
         #include "BehaviorTree/BlackboardComponent.h"

         UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
         {
            NodeName = TEXT("CanAttack");	//데코레이터의 이름이 된다.
         }

         bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
         {
            bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

            auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
            if(nullptr == ControllingPawn)	return false;

            auto Target = Cast<AABCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
            if(nullptr == Target) return false;

            bResult = (Target->GetDistanceTo(ControllingPawn) <= 200.0f);
            return bResult;
         }
         ```

         </details>

   2. 대기가 아닌 공격의 구현
      - <img src="Image/IsInAttack.gif" height="300" title="IsInAttack">
      - BTTaskNode를 부모로 하는 BTTask_Attack이라는 클래스를 생성하고, FinishLatentTask 함수를 사용한다. -> Tick 사용
         - 이 공격 태스크는 애니메이션이 끝날때까지 대기해야하는 지연 태스크임으로 ExecuteTask를 Inprogess로 반환하고 끝날 시 알려줘야한다.
      - AI컨트롤러에서도 공격 명령을 내릴 수 있도록 ABCharater 클래스의 Attack함수를 public으로 변경하고 델리게이트를 새로 선언한다. 공격이 종료될 때 이를 호출한다.
      - 태스크에서 람다 함수를 해당 델리게이트에 등록하고 Tick 함수 로직에서 이를 파악해 FinishLatentTask함수를 호출하여 태스크를 종료한다.

         <details><summary>코드 보기</summary>

         ```c++
         //BTTask_Attack.h
         public:
            UBTTask_Attack();

            virtual  EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

         protected:
            virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
         
         private:
            bool IsAttacking = false;
            
         //BTTask_Attack.cpp
         #include "BTTask_Attack.h"
         #include "ABAIController.h"
         #include "ABCharacter.h"

         UBTTask_Attack::UBTTask_Attack()
         {
            bNotifyTick = true;	//tick 사용
            IsAttacking = false;
         }

         EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
         {
            Super::ExecuteTask(OwnerComp, NodeMemory);

            auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
            if(nullptr == ABCharacter) return EBTNodeResult::Failed;

            ABCharacter->Attack();
            IsAttacking = true;
            ABCharacter->OnAttackEnd.AddLambda([this]()-> void
            {
               IsAttacking = false;
            });
            
            return EBTNodeResult::InProgress;
         }

         void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
         {
            Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
            if(!IsAttacking)
            {
               FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            }
         }
         //ABCHaracter.h
         DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);
         ...
         public:
            void Attack();
            FOnAttackEndDelegate OnAttackEnd;
         //ABCHaracter.cpp
         void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
         {
            ABCHECK(IsAttacking);
            ABCHECK(CurrentCombo > 0);
            IsAttacking = false;
            AttackEndComboState();
            OnAttackEnd.Broadcast();
         }
         ```

         </details>
   3. 플레이어를 향해 회전하는 기능 추가
      - <img src="Image/Attack.gif" height="300" title="Attack">
      -위 사진을 보면 회전에 있어 오류가 있기에 BTTaskNode를 부모로 하는 BTTask_TurnToTarget이라는 태스크를 생성하고 FMath::RInterpTo 함수를 사용한다.

         <details><summary>코드 보기</summary> 
           
         ```c++
         //BTTask_TurnToTarget.h
         public:
            UBTTask_TurnToTarget();
            virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
         //BTTask_TurnToTarget.cpp
         #include "BTTask_TurnToTarget.h"
         #include "ABAIController.h"
         #include "ABCharacter.h"
         #include "BehaviorTree/BlackboardComponent.h"

         UBTTask_TurnToTarget::UBTTask_TurnToTarget()
         {
            NodeName = TEXT("Turn");
         }

         EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
         {
            Super::ExecuteTask(OwnerComp, NodeMemory);

            auto ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
            if(nullptr == ABCharacter) return EBTNodeResult::Failed;

            auto Target = Cast<AABCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
            if(nullptr == Target) return EBTNodeResult::Failed;

            FVector LookVector = Target->GetActorLocation() - ABCharacter->GetActorLocation();
            LookVector.Z = 0.0f;
            FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
            ABCharacter->SetActorRotation(FMath::RInterpTo(ABCharacter->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(),2.0f));

            return EBTNodeResult::Succeeded;
         }
         ```

         </details>
   
- ### 프로젝트의 정리와 모듈의 추가
   - 헤더파일은 모두 Public 폴더로 C++파일은 모두 Private 폴더로 옮겨주었다.
   - 언리얼엔진은 게임 모듈을 사용해 게임 프로젝트의 로직을 관리한다. 지금까지는 Arena라는 주 게임 모듈에서 관리했다. 다른 모듈을 게임을 프로젝트에 추가하려면 로직을 분리해야한다.
      - 기존 제공받은 ArenaBattleSetting파일을 Source에 추가하고 모듈을 빌드하도록 Arena.Target.cs와 ArenaEditor.Target.cs를 수정하여 게임빌드와 에디터 빌드 설정을 지정해준다.
   
      ```c++
      //Arena.Target.cs 와 ArenaEditor.Target.cs
      ExtraModuleNames.AddRange( new string[] { "Arena", "ArenaBattleSetting" } );
      ```

   - <img src="Image/Win64.png" height="300" title="Win64">
   - 위 과정을 마치고 빌드가 완료되면 Binaries 폴더에 새로운 파일(dll)이 만들어진다.
      - 생성되면 언리얼 에디터가 이 DLL 파일을 uproject 파일을 통해서 로딩하도록 명령한다. 
      - PreDefault로 설정하여 기존 Arena가 ArenaBattleSetting에게 의존하도록한다. -> ArenaBattleSetting이 먼저 프로세스에 올라감
   - 이 과정이 새로운 모듈을 추가하는 방법이다.


> **<h3>Realization</h3>**
- 데코레이터의 NodeName은 비헤이비어 트리에 나타나게되는 이름이다.
- 언리얼엔진은 게임 모듈을 사용해 게임 프로젝트의 로직을 관리한다.다른 모듈을 게임을 프로젝트에 추가하려면 로직을 분리해야한다.
   - 주 게임 모듈은 자동으로 생성되지만 추가 모듈은 따로 추가한다.
   - 모듈 폴더와 빌드 설정 파일 : 모듈 폴더와 모듈명으로 된 Build.cs파일
   - 모듈의 정의 파일 : 모듈명으로 된 .cpp 파일

## **06.07**
> **<h3>Today Dev Story</h3>**
- ### INI 설정과 애셋의 지연 로딩
   1. 새로운 모듈에 추가한 ABCharacterSetting은 캐릭터 애셋의 목록을 보관한다. 이때 외부 INI파일을 사용한다.
      - 생성자 코드에 지정할 수 있지만 애셋이 변경되면 코드를 다시 만들고 컴파일해야하는 번거롭기 때문에 유연하게 관리하도록 외부 INI파일에서 기본 속성 값을 지정하는 기능을 사용한다.
      - 애셋 경로를 보관하기 위해 FSoftObjectPath라는 클래스를 제공한다. UCLASS 매크로에 config키워드 추가해 불러들일 INI파일의 이름을 지정하고 불러들일 PROPERTY 속성에는 config 키워드를 선언한다. 
      - UCLASS 매크로내 config 키워드의 Arena이라는 설정으로 인해 초기화 단계에서 Config에 있는 DefaultArena.ini파일을 만들어 ABCharacterSetting의 CharacterAssets값을 설정한다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABCharacterSetting.h
      UCLASS(config=Arena)
      class ARENABATTLESETTING_API UABCharacterSetting : public UObject
      {
         GENERATED_BODY()
         
      public:
         UABCharacterSetting();
         
         UPROPERTY(config)
         TArray<FSoftObjectPath> CharacterAssets;
      };
      ```

      </details>

   2. Config 폴더에 DefaultArena.ini 파일을 넣어준다. 클래스 기본 객체가 모두 메모리에 올라간다.
      - <img src="Image/GetDefault.png" height="250" title="GetDefault">
         
         - 이렇게 메모리에 올라간 클래스 기본 객체는 GetDefault 함수를 사용해 가져올 수 있다. (엔진 종료시까지)
         - Arena.Buil.cs에서 모듈을 참조하기 위해 추가해준다.
            
            ```c++
            PrivateDependencyModuleNames.AddRange(new string[] { "ArenaBattleSetting" });
            ```

         - GetDefault 함수를 사용해 애셋 목록을 읽어들인다.
      
         <details><summary>코드 보기</summary>

         ```c++
         //ABCharacter.cpp
         #include "ABCharacterSetting.h"
         AABCharacter::AABCharacter()
         {
            ...
            auto DefaultSetting = GetDefault<UABCharacterSetting>();
            if(DefaultSetting->CharacterAssets.Num() > 0)
            {
               for (auto CharacterAsset : DefaultSetting->CharacterAssets)
               {
                  ABLOG(Warning, TEXT("Character Asset : %s"),*CharacterAsset.ToString());
               }
            }
         }
         ```

         </details>

   3. NPC가 생성될때 랜덤하게 목록 중 하나를 골라 로딩하도록 변경
      - <img src="Image/Rand_NPC.gif" height="300" title="Rand_NPC">
      - 게임 중에도 비동기 방식으로 에셋을 로딩하도록 FStreamableManager라는 클래스를 지원한다.
         - 이 클래스는 프로젝트에서 하나만 활성화 하는 것이 좋기에 ABGameInstance에서 멤버 변수로 선언한다. 
         - 로딩 명령은 AsyncLoad이다. FStreamableDelegate로 가능하지만 CreateUObject 명령을 사용해 즉석에서 델리게이트를 생성하여 함수와 연동하고 넘겨주는 방식이 간편하다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABGameInstance.h
      #include "Engine/StreamableManager.h"
      ...
      public:
         FStreamableManager StreamableManager;

      //ABCharacter.h
      private:
         ...
      	void OnAssetLoadCompleted();
         	
         FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);
	      TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;

      //ABCharacter.cpp
      void AABCharacter::BeginPlay()
      {
         Super::BeginPlay();

         if(!IsPlayerControlled())
         {
            auto DefaultSetting = GetDefault<UABCharacterSetting>();
            int32 RandIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
            CharacterAssetToLoad = DefaultSetting->CharacterAssets[RandIndex];

            auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
            if(nullptr != ABGameInstance)
            {
               AssetStramingHandle = ABGameInstance->StreamableManager->RequestAsyncLoad(CharacterAssetToLoad,FStreamableDelegate::CreateUObject(this,&AABCharacter::OnAssetLoadCompleted));
            }
         }
         ...
      }
      ...
      void AABCharacter::OnAssetLoadCompleted() //실질적으로 변경
      {
         USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
         AssetStreamingHandle.Reset();
         if(nullptr != AssetLoaded)
         {
            GetMesh()->SetSkeletalMesh(AssetLoaded);
         }
      }
      ```

      </details>

- ### 무한 맵의 생성
   - 하나의 섹션을 클리어하면 새로운 섹션이 등장하는 무한 맵 스테이지를 제작한다. 특징은 다음과 같다.
      - 섹션의 배경과 네 방향으로 캐릭터 입장을 통제하는 문을 제공한다.
      - 플레이어가 섹션에 진입하면 모든 문을 닫는다.
      - 문을 닫고 일정 시간 후 중앙에 NPC를 생성, 랜덤한 위치에 아이템 상자 생성
      - 생성한 NPC가 죽으면 모든 문을 개방, 통과한 문으로 이어지는 새로운 섹션 생성
   - Actor를 부모로 하는 ABSection 클래스를 Arena 모듈에 생성한다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABSection.h
      private:
         UPROPERTY(VisibleAnywhere, Category=MESHBUILDER_API, Meta=(AllowPrivateAccess = true))
         UStaticMeshComponent *Mesh;
      //ABSection.cpp
      AABSection::AABSection()
      {
         // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
         PrimaryActorTick.bCanEverTick = false;


         Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
         RootComponent = Mesh;

         FString AssetPath = TEXT("/Game/Book/StaticMesh/SM_SQUARE.SM_SQUARE");
         static ConstructorHelpers::FObjectFinder<UStaticMesh>
         SM_SQUARE(*AssetPath);
         if(SM_SQUARE.Succeeded())
         {
            Mesh->SetStaticMesh(SM_SQUARE.Object);
         }
         else
         {
            ABLOG(Error, TEXT("Failed to load staticmesh asset : %s"),*AssetPath);
         }
      }
      ```

      </details>

   - <img src="Image/Gate_Collision.png" height="300" title="Gate_Collision">
   - 준비한 애셋에는 출입문을 붙일 수 있게 소켓이 준비되어 있는데 이에 철문을 부착한다.
      - 철문마다 스태틱메시 컴포넌트를 제작하고 이를 소켓에 부착한다.
      - 소켓 목록을 제작하고 이를 사용해 철문을 각각 부착한다. (TArray)
      - <img src="Image/ABTrigger.png" height="300" title="ABTrigger">
         - ABCharacter만을 감지하는 ABTrigger 콜리전 프리셋을 만들어 box 컴포넌트로 섹션에 추가한다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABSection.h
      public:	
         UPROPERTY(VisibleAnywhere, Category=Mesh, Meta = (AllowPrivateAccess = true))
         TArray<UStaticMeshComponent*> GateMeshes;	//배열 여러개

         UPROPERTY(VisibleAnywhere, Category=Trigger, Meta = (AllowPrivateAccess = true))
	      TArray<UBoxComponent*> GateTriggers;

      private:
         UPROPERTY(VisibleAnywhere, Category=Trigger, Meta=(AllowPrivateAccess = true))
         UBoxComponent *Trigger;
         
      //ABSection.cpp
      AABSection::AABSection()
      {
         ...
         Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
         Trigger->SetBoxExtent(FVector(775.0f,775.0f,300.0f));
         Trigger->SetupAttachment(RootComponent);
         Trigger->SetRelativeLocation(FVector(0.0f,0.0f,250.0f));
         Trigger->SetCollisionProfileName(TEXT("ABTrigger"));

         FString GateAssetPath = TEXT("/Game/Book/StaticMesh/SM_GATE.SM_GATE");
         static ConstructorHelpers::FObjectFinder<UStaticMesh>
         SM_GATE(*GateAssetPath);
         if(!SM_GATE.Succeeded())
         {
            ABLOG(Error, TEXT("Failed to load staticmesh asset : %s"),*GateAssetPath);
         }

         static FName GateSockets[] = {{TEXT("+XGate")},{TEXT("-XGate")},{TEXT("+YGate")},{TEXT("-YGate")}};
         for (FName GateSocket : GateSockets)
         {
            ABCHECK(Mesh->DoesSocketExist(GateSocket));
            UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*GateSocket.ToString());
            NewGate->SetStaticMesh(SM_GATE.Object);
            NewGate->SetupAttachment(RootComponent,GateSocket);
            NewGate->SetRelativeLocation(FVector(0.0f,-80.5f,0.0f));
            GateMeshes.Add(NewGate);

            UBoxComponent* NewGateTrigger = CreateDefaultSubobject<UBoxComponent>(*GateSocket.ToString().Append(TEXT("Trigger")));
            NewGateTrigger->SetBoxExtent(FVector(100.0f,100.0f,300.0f));
            NewGateTrigger->SetupAttachment(RootComponent,GateSocket);
            NewGateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f,250.0f));
            NewGateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
            GateTriggers.Add(NewGateTrigger);
         }
      }
      ```

      </details>


> **<h3>Realization</h3>**
- 외부 INI설정을 통해 오브젝트의 기본값을 유연하기 관리할 수 있다. 
   - 애셋 경로를 보관하기 위해 FSoftObjectPath라는 클래스를 제공한다. UCLASS 매크로에 config키워드 추가해 불러들일 INI파일의 이름을 지정하고 불러들일 PROPERTY 속성에는 config 키워드를 선언한다. 
   - 이렇게 메모리에 올라간 클래스 기본 객체는 GetDefault 함수를 사용해 가져올 수 있다. (엔진 종료시까지)
- 클래스 기본 객체 : 모듈이 로딩되면서 자신에게 속한 오브젝트의 기본값을 지정해 생성하는 과정.
- 게임 중에도 비동기 방식으로 에셋을 로딩하도록 FStreamableManager라는 클래스를 지원한다.
   - 이 클래스는 프로젝트에서 하나만 있는 것이 좋다.

## **06.08**
> **<h3>Today Dev Story</h3>**
- ### 무한 맵의 생성_2
   1. 철문 액터의 로직을 스테이트 머신으로 제작한다. (열거형으로 표현) 
      - <img src="Image/Door_Open.png" height="300" title="Door_Open">
         - 준비 : 문을 열어놓고 대기하다가 중앙의 박스 트리거로 플레어의 진입을 감지하면 전투 상태로 이동
         - 전투 : 문을 닫고 일정 시간이 지난면 NPC와 아이템 상자 생성. 모든 NPC가 죽으면 완료 상태로 이동
         - 완료 : 닫힌 문을 열고 각 문에 배치되어 있는 트리거 게이트로 플레어이를 감지하며 이동한 문의 방향으로 새로운 섹션을 소환
      -  OnConstruction함수를 활용하여 스테이트 상황으로 모든 문이 열리도록 제작자에게 보여준다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABSection.h
         public :
            virtual void OnConstruction(const FTransform& Transform) override;
         private:
            enum class EsectionState : uint8
            {
               READY = 0,
               BATTLE,
               COMPLETE
            };

            void SetState(EsectionState NewState);
            EsectionState CurrentState = EsectionState::READY;

            void OperateGates(bool bOpen = true);

            UPROPERTY(EditAnywhere, Category=State, Meta=(AllowPrivateAccess = true))
            bool bNoBattle;
         
         //ABSection.cpp
         void AABSection::BeginPlay()
         {
            Super::BeginPlay();

            SetState(bNoBattle? EsectionState::COMPLETE : EsectionState::READY);	//현 상태에 따라 스테이트 조정
         }

         void AABSection::SetState(EsectionState NewState)
         {
            switch (NewState)
            {
            case EsectionState::READY:
               Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
               for(UBoxComponent* GateTrigger : GateTriggers) GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
               OperateGates(true);
               break;
            case EsectionState::BATTLE:
               Trigger->SetCollisionProfileName(TEXT("NoCollision"));
               for(UBoxComponent* GateTrigger : GateTriggers) GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
               OperateGates(false);
               break;
            case EsectionState::COMPLETE:
               Trigger->SetCollisionProfileName(TEXT("NoCollision"));
               for(UBoxComponent* GateTrigger : GateTriggers) GateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
               OperateGates(true);
               break;
            }
            CurrentState = NewState;
         }

         void AABSection::OperateGates(bool bOpen)
         {
            for (UStaticMeshComponent* Gate : GateMeshes) Gate->SetRelativeRotation(bOpen ? FRotator(0.0f,-90.0f,0.0f) : FRotator::ZeroRotator);
         }
         ```

         </details>

   2. 문을 지나면 새로운 섹션 액터의 생성
      - <img src="Image/Auto_Section.gif" height="300" title="Auto_Section">
      - 이미 존재할 수도 있기에 확인하는 로직 또한 필요하다. (물리 엔진 기능 사용하며 이미 존재시 생성을 건너뜀)
      - 새로운 섹션은 READY 상태부터 시작하며 트리거 영역을 활성화해서 플레이어의 진입을 감지한다. 또한 BATTLE 스테이트로 전환하고 문을 닫는다.
      - 박스 컴포넌트를 사용하며 OnComponentBeginOverlap 델리게이트에 바인할 함수를 생성하고 연결한다.
         - 모든 문을 하나의 델리게이트 함수에 연결한다. 이때 어떤 문인지 구분할 수 있도록 소켓에 태그를 설정하며 방향에 띄울 다음 섹션까지 생성하는 기능을 구현한다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABSection.h
         UFUNCTION()
         void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
      
         UFUNCTION()
         void OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

         //ABSection.cpp
         AABSection::AABSection()
         {
            ...
            Trigger->OnComponentBeginOverlap.AddDynamic(this, & AABSection::OnTriggerBeginOverlap);
            ...
            for (FName GateSocket : GateSockets)
            {
               ...
               NewGateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnGateTriggerBeginOverlap);
               NewGateTrigger->ComponentTags.Add(GateSocket);
            }
            ...
         }
         ...
         void AABSection::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
         {
            if(CurrentState == EsectionState::READY) SetState(EsectionState::BATTLE);
         }

         //생성
         void AABSection::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
         {
            ABLOG(Warning,TEXT("The map produced"));
            ABCHECK(OverlappedComponent->ComponentTags.Num() == 1);

            FName ComponentTag = OverlappedComponent->ComponentTags[0];
            FName SocketName = FName(*ComponentTag.ToString().Left(2));
            if(!Mesh->DoesSocketExist(SocketName)) return;

            FVector NewLocation = Mesh->GetSocketLocation(SocketName);

            TArray<FOverlapResult> OverlapResults;
            FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);
            FCollisionObjectQueryParams ObjectQueryParam(FCollisionObjectQueryParams::InitType::AllObjects);

            bool bResult = GetWorld()->OverlapMultiByObjectType(
               OverlapResults,
               NewLocation,
               FQuat::Identity,
               ObjectQueryParam,
               FCollisionShape::MakeSphere(775.0f),
               CollisionQueryParam
            );

            if(!bResult) auto NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation,FRotator::ZeroRotator);
            else ABLOG(Warning, TEXT("New section area is not empty"));
         }
         ```

         </details>

- ### 내비게이션 메시 시스템 설정 
   - <img src="Image/Generate_Section_End.gif" height="300" title="Generate_Section_End">
   - 섹션에서 NPC와 아이템 상자를 생성하는 기능을 추가 (타이머를 이용해 생성될 시간조정)
      - 2초후에 NPC가 생성되고 5초 후에는 NPC반경 6미터 내의 랜덤한 위치에 아이템 생성. 
   - 게임중에 동적으로 내비게이션 메시를 생성하도록 설정해 줘야 새로운 섹션에서도 움직인다.
      - 프로젝트 세팅 > 내비게이션 메시 > Runtime Generation을 Dynamic으로 변경
      <details><summary>코드 보기</summary>

      ```c++
      //ABSection.h
      private:
         void OnNPCSpawn();
         UPROPERTY(EditAnywhere, Category=Spawn, Meta=(AllowPrivateAccess = true))
         float EnemySpawnTime;

         UPROPERTY(EditAnywhere, Category=Spawn, Meta=(AllowPrivateAccess = true))
         float ItemBoxSpawnTime;

         FTimerHandle SpawnNPCTimerHandle = {};
         FTimerHandle SpawnItemBoxTimerHandle = {};
      //ABSection.cpp
      #include "ABCharacter.h"
      #include "ABItemBox.h"

      AABSection::AABSection()
      {
         ...
      	EnemySpawnTime = 2.0f;
         ItemBoxSpawnTime = 5.0f;
         ...
      }
      ...
      void AABSection::SetState(EsectionState NewState)
      {
         ...
         switch (NewState)
	      {
         case EsectionState::BATTLE:
            ...
            GetWorld()->GetTimerManager().SetTimer(SpawnNPCTimerHandle,FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn),EnemySpawnTime, false);
            GetWorld()->GetTimerManager().SetTimer(SpawnItemBoxTimerHandle, FTimerDelegate::CreateLambda([this]()->void{
               FVector2D RandXY = FMath::RandPointInCircle(600.0f);
               GetWorld()->SpawnActor<AABItemBox>(GetActorLocation() + FVector(RandXY,30.0f),FRotator::ZeroRotator);
            }), ItemBoxSpawnTime, false);
            
            break;
         }
      }
      ...
      void AABSection::OnNPCSpawn() //NPC의 생성
      {
         GetWorld()->SpawnActor<AABCharacter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator);
      }
      ```

      </details>

> **<h3>Realization</h3>**
- 에디터 작업에서 선택한 액터의 속성이나 트랜스폼 정보가 변경될때 OnConstruction함수가 실행된다.
     

## **06.09**
> **<h3>Today Dev Story</h3>**
- ### 캐릭터 스테이트 설정
   - <img src="Image/Auto_Reset.gif" height="300" title="Auto_Reset">
   1. 캐릭터의 기능을 체계적으로 관리하기 위해 캐릭터에 스테이트 머신 모델을 구현한다. (열거형)
      - PREINIT : 캐릭터 생성 전의 상태. 설정은 되어 있지만 캐릭터와 UI를 숨긴다. 그리고 데미지를 입지 않는다.
      - LOADING : 선택한 캐릭터 애셋을 로딩하는 산태. 시작된 시점이기 때문에 구분이 가능하다. 플레이어 컨트롤러는 조종을 못하게 한다.
      - READY : 로딩이 완료된 상태. 숨겨진 캐릭터와 UI를 보며주며 데미지를 받는다.
      - DEAD : 캐릭터가 HP를 소진해 사망한 상태. 죽는 애니메이션을 재생하고 UI를 끈다. 동시에 충돌 기능을 없애고 입력을 비활성화한다. AI인 경우 비헤이비어 트리 로직을 중단하고 시간이 지나면 모든 상태를 초기화하고 재시작한다.
      - 블루프린트에서 사용할 수 있도록 UENUM(BlueprintType)을 선언하고 uint8으로 기반 유형을 지정

         <details><summary>코드 보기</summary>

         ```c++
         //Arena.h
         UENUM(BlueprintType)
         enum class ECharacterState : uint8
         {
            PREINIT,
            LOADING,
            READY,
            DEAD
         };
         //ArenaCharacter.h
         public:
            AABCharacter();
            void SetCharacterState(ECharacterState NewState);
            ECharacterState GetCharacterState() const;
         private:
            //상황
            int32 AssetIndex = 0;
            
            UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category=State, Meta=(AllowPrivateAccess = true))
            ECharacterState CurrentState;
            
            UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category=State, Meta=(AllowPrivateAccess = true))
            bool bIsPlayer;

            UPROPERTY()
            class AABAIController* ABAIController;

            UPROPERTY()
            class AABPlayerController* ABPlayerController;
         ```

         </details>


   2. 각 스테이트에 해야하는 행동을 설정
      - 현재 설정상 플레이어도 전용 AI컨트롤러가 부착되므로 플레이어의 판별은 BeginPlay 시점이다. 이 시점에서 파악 후 IsPlayer변수에 결과를 저장한다.
      - PREINIT 상태에서 시작되며 애셋의 로딩을 진행하고 LOADING으로 변경, 로딩이 완료되면 READY에서 게임을 진행하고 HP가 0이하로 떨어지면 DEAD로 변경
      - 플레이어의 경우 INDEX 4번인 캐릭터 애셋을 사용하고 나머지는 랜점을 사용한다.
        
         <details><summary>코드 보기</summary>

         ```c++
         //ABCharacter.cpp
         #include "ABPlayerController.h"
         AABCharacter::AABCharacter()
         {
            ...
            //스테이트에 따른 설정
            AssetIndex = 4;

            //숨긴다.
            SetActorHiddenInGame(true);
            HPBarWidget->SetHiddenInGame(true);
            SetCanBeDamaged(false);
         } 

         void AABCharacter::BeginPlay()
         {
            Super::BeginPlay();
            
            auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
            ABCHECK(nullptr != CharacterWidget);
            CharacterWidget->BindCharacterStat(CharacterStat);

            auto DefaultSetting = GetDefault<UABCharacterSetting>();
            bIsPlayer = IsPlayerControlled();
            if(bIsPlayer)	//플레이어인 경우
            {
               ABPlayerController = Cast<AABPlayerController>(GetController()());
               ABCHECK(nullptr != ABPlayerController);
               AssetIndex = 4;	//생성에셋
            }
            else	//Ai인 경우
            {
               ABAIController = Cast<AABAIController>(GetController());
               ABCHECK(nullptr != ABAIController);
               AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);	//생성에셋
            }
            
            if(!IsPlayerControlled())
            {
               auto DefaultSetting = GetDefault<UABCharacterSetting>();
               int32 RandIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
               CharacterAssetToLoad = DefaultSetting->CharacterAssets[RandIndex];

               auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
               if(nullptr != ABGameInstance)
               {
                  AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
               }
            }
            
            CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
            auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
            ABCHECK(nullptr != ABGameInstance);
            AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
            SetCharacterState(ECharacterState::LOADING);	//로딩으로 전환
         }

         void AABCharacter::SetCharacterState(ECharacterState NewState)
         {
            ABCHECK(CurrentState != NewState);
            CurrentState = NewState;

            switch (CurrentState)
            {
            case ECharacterState::LOADING:
               SetActorHiddenInGame(true);
               HPBarWidget->SetHiddenInGame(true);
               SetCanBeDamaged(false);
               break;
            case ECharacterState::READY:
               SetActorHiddenInGame(false);
               HPBarWidget->SetHiddenInGame(false);
               SetCanBeDamaged(true);
               CharacterStat->OnHpIsZero.AddLambda([this]()->void
               {
                  SetCharacterState(ECharacterState::DEAD);
               });
               break;
            case ECharacterState::DEAD:
               SetActorEnableCollision(false);
               GetMesh()->SetHiddenInGame(false);
               HPBarWidget->SetHiddenInGame(true);
               ABAnim->SetDeadAnim();
               SetCanBeDamaged(false);
               break;
            }
         }

         ECharacterState AABCharacter::GetCharacterState() const
         {
            return CurrentState;
         }
         void AABCharacter::OnAssetLoadCompleted()
         {
            ...
            SetCharacterState(ECharacterState::READY);
         }
         ```

         </details>
   
   3. 스테이트에 맞게 비헤이비어 트리 로직을 수동으로 구동하고 중지할 수 있게 AI컨트롤러의 구조를 변경한다.
      <details><summary>코드 보기</summary>

      ```c++
      //ABAIController.h
      public:
      	void RunAI();
	      void StopAI();
      //ABAIController.cpp
      void AABAIController::OnPossess(APawn* InPawn)
      {
         Super::OnPossess(InPawn);
      }

      void AABAIController::RunAI()
      {
         if(UseBlackboard(BBAsset,Blackboard))
         {
            Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());	//Location을 HomePosKey에..
            if(!RunBehaviorTree(BTAsset)) ABLOG(Error,TEXT("AIController couldn't run behavior tree!"));
         }
      }

      void AABAIController::StopAI()
      {
         auto BehavoirTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
         if(nullptr != BehavoirTreeComponent) BehavoirTreeComponent->StopTree(EBTStopMode::Safe);	//없다면 정지
      }
      ```

      </details>
   
   4. 플레이어가 빙의할때 발생하는 PossessedBy함수는 제거하고 캐릭터의 READY 스테이트에서 구현한다. 
      - 플레이어인 경우 입력을 활성화, AI인 경우 비헤이비어 트리 활성화. 죽으면 비활성화

         <details><summary>코드 보기</summary>

         ```c++
         //ABCharacter.h
         private:
            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=State,Meta=(AllowPrivateAccess = true))
            float DeadTimer;

            FTimerHandle DeadTimerHandle = {};
         //ABCharacter.cpp
         void AABCharacter::SetCharacterState(ECharacterState NewState)
         {
            ...
            switch (CurrentState)
            {
            case ECharacterState::LOADING:
               if(bIsPlayer) DisableInput(ABPlayerController);
               ...
            case ECharacterState::READY:
               ...

               if(bIsPlayer)	//Player
               {
                  SetControlMode(EControlMode::DIABLO);
                  GetCharacterMovement()->MaxWalkSpeed = 600.0f;
                  EnableInput(ABPlayerController);
               }
               else	//AI
               {
                  SetControlMode(EControlMode::NPC);
                  GetCharacterMovement()->MaxWalkSpeed = 400.0f;
                  ABAIController->RunAI();
               }
               break;
            case ECharacterState::DEAD:
               ...

               if(bIsPlayer) DisableInput(ABPlayerController);
               else ABAIController->StopAI();

               GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void
               {
                  if(bIsPlayer) ABPlayerController->RestartLevel();
                  else Destroy();
               }),DeadTimer, false);
               
               break;
            }
         }
         ```

         </details>

- ### 플레이어 데이터와 UI연동
   1. 게임 점수와 같은 플레이어의 게임 데이터를 별도의 액터에서 보관하도록 구현한다. (PlayerState를 상속 받은 ABPlayerStaet)
      - 이 클래스에는 FString형의 PlayerName 속성과 float형의 Score 속성이 미리 설계되어 있지만 따로 게임 진행 상황을 int32형으로 GameScore와 레벨을 ChacterLevel이라는 속성을 추가한다.
      - 이 클래스를 게임 모드의 PlayerStateClass 속성에 지정하면 컨트롤러가 초기화될 때 함께 인스턴스를 생성하고 값을 컨트롤러의 PlayerState속성에 저장한다. PostLogin함수에 구성이 완료되므로 ABPlayerState의 초기화도 동시에 진행한다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABPlayerState.h
      class ARENA_API AABPlayerState : public APlayerState
      {
         GENERATED_BODY()

      public:
         AABPlayerState();

         int32 GetGameScore() const;
         int32 GetCharacterLevel() const;

         void InitPlayerData();

      protected:
         UPROPERTY(Transient)
         int32 GameScore;	//점수

         UPROPERTY(Transient)
         int32 CharacterLevel;	//레벨
      };
      //ABPlayerState.cpp
      AABPlayerState::AABPlayerState()
      {
         CharacterLevel = 1;
         GameScore = 0;
      }

      int32 AABPlayerState::GetGameScore() const
      {
         return GameScore;
      }

      int32 AABPlayerState::GetCharacterLevel() const
      {
         return CharacterLevel;
      }

      void AABPlayerState::InitPlayerData()	//초기 생성
      {
         SetPlayerName(TEXT("Destiny"));
         CharacterLevel = 5;
         GameScore = 0;
      }
      //ABGameMode.cpp
      #include "ABPlayerState.h"

      AABGameMode::AABGameMode()
      {
         DefaultPawnClass = AABCharacter::StaticClass();	//ABPawn의 클래스 정보를 저장 (멀티 플레이를 고려, 만들어두는 것이 아님)

         PlayerControllerClass = AABPlayerController::StaticClass();
         PlayerStateClass = AABPlayerState::StaticClass();
      }

      void AABGameMode::PostLogin(APlayerController* NewPlayer)
      {
         Super::PostLogin(NewPlayer);

         auto ABPlayerState = Cast<AABPlayerState>(NewPlayer->PlayerState);
         ABCHECK(nullptr != ABPlayerState);
         ABPlayerState->InitPlayerData();	//초기화
      }
      ```

      </details>

   2. 위의 정보를 토대로 레벨정보는 실제로 플레이어에게 반영해야한다
      - <img src="Image/Start_5.png" height="300" title="Start_5"> 
      - 컨트롤러가 캐릭터에 빙의할때 PlayerState속성에 플레이어 스테이트 포인터를 저장하기에 캐릭터에서 바로 가져올 수 있다.
      - 시작시 5레벨로 진행한다.
       
         <details><summary>코드 보기</summary>

         ```c++
         void AABCharacter::SetCharacterState(ECharacterState NewState)
         {
            ...
            switch (CurrentState)
            {
            case ECharacterState::LOADING:
               if(bIsPlayer) 
               {
                  DisableInput(ABPlayerController);

                  auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
                  ABCHECK(nullptr != ABPlayerState);
                  CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());
               }
               ...
            }
         }
         ```

         </details>

   3. 위의 정보를 확인할 수 있는 HUD UI를 화면에 생성한다.
      - <img src="Image/StateWidget.png" height="300" title="StateWidget"> 
      - UserWidget을 부모로 하는 ABHUDWidget을 생성하고 UI_HUD 애셋의 그래프 > ABHUDWidget을 부모로 설정한다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABPlayerController.h
         public: 
            AABPlayerController();
            ...
            class UABHUDWidget* GetHUDWidget() const;
         protected:
            ...
            UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=UI)
            TSubclassOf<class UABHUDWidget> HUDWidgetClass;

         private:
            UPROPERTY()
            class UABHUDWidget* HUDWidget;
         //ABPlayerController.cpp
         #include "ABHUDWidget.h"

         AABPlayerController::AABPlayerController()
         {
            static ConstructorHelpers::FClassFinder<UABHUDWidget>
            UI_HUD_C(TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
            if(UI_HUD_C.Succeeded()) HUDWidgetClass = UI_HUD_C.Class;
         }
         void AABPlayerController::BeginPlay()
         {
            Super::BeginPlay();

            FInputModeGameOnly InputMode;
            SetInputMode(InputMode);

            HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
            HUDWidget->AddToViewport();
         }

         UABHUDWidget * AABPlayerController::GetHUDWidget() const
         {
            return HUDWidget;
         }
         ```

         </details>

- ### UI와 데이터의 연동 
   - <img src="Image/Data_Connection.gif" height="300" title="Data_Connection">
   1. 플레이어 스테이트와 캐릭터 스탯 컴포넌트 정보를 모두 해당 HUB에 연동한다.
      - 새로운 델리게이트를 정의하고 플레이어 데이터가 변동될때 HUD에 신호를 보내 업데이트 하도록한다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABPlayerState.h
         DECLARE_MULTICAST_DELEGATE(FOnPlayerStateChangedDelegate);
         public:
            FOnPlayerStateChangedDelegate OnplayerStateChanged;
         //ABHUDWidget.h
         class ARENA_API UABHUDWidget : public UUserWidget
         {
            GENERATED_BODY()

         public:
            void BindCharcterStat(class UABCharacterStatComponent* CharacterStat);
            void BindPlayerStat(class AABPlayerState* PlayerState);

         protected:
            virtual void NativeConstruct() override;
            void UpdateCharacterStat();
            void UpdatePlayerState();

         private:
            TWeakObjectPtr<class UABCharacterStatComponent> CurrentCharacterStat;
            TWeakObjectPtr<class AABPlayerState> CurrentPlayerState;

            UPROPERTY()
            class UProgressBar* HPBar;

            UPROPERTY()
            class UProgressBar* ExpBar;

            UPROPERTY()
            class UTextBlock* PlayerName;

            UPROPERTY()
            class UTextBlock* PlayerLevel;

            UPROPERTY()
            class UTextBlock* CurrentScore;

            UPROPERTY()
            class UTextBlock* HighScore;
         };
         
         //ABHUDWidget.cpp
         #include "ABHUDWidget.h"
         #include "Components/ProgressBar.h"
         #include "Components/TextBlock.h"
         #include "ABCharacterStatComponent.h"
         #include "ABPlayerState.h"

         void UABHUDWidget::BindCharcterStat(UABCharacterStatComponent* CharacterStat)	//HP바
         {
            ABCHECK(nullptr != CharacterStat);
            CurrentCharacterStat = CharacterStat;
            CharacterStat->OnHpChanged.AddUObject(this, &UABHUDWidget::UpdateCharacterStat);	//델리게이트
         }
         void UABHUDWidget::BindPlayerStat(AABPlayerState* PlayerState)	//상태 -> 싸움?
         {
            ABCHECK(nullptr != PlayerState);
            CurrentPlayerState = PlayerState;
            PlayerState->OnplayerStateChanged.AddUObject(this, &UABHUDWidget::UpdatePlayerState);
         }
         void UABHUDWidget::NativeConstruct()
         {
            Super::NativeConstruct();
            HPBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbHP")));
            ABCHECK(nullptr != HPBar);

            ExpBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("pbEXP")));
            ABCHECK(nullptr != ExpBar);

            PlayerName = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtPlayerName")));
            ABCHECK(nullptr != PlayerName);

            PlayerLevel = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtPlayerLevel")));
            ABCHECK(nullptr != PlayerLevel);

            CurrentScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtCurrentScore")));
            ABCHECK(nullptr != CurrentScore);

            HighScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtHighScore")));
            ABCHECK(nullptr != HighScore);
         }


         void UABHUDWidget::UpdateCharacterStat()	//HP바에 색칠
         {
            ABCHECK(CurrentCharacterStat.IsValid());

            HPBar->SetPercent(CurrentCharacterStat->GetHPRatio());
         }

         void UABHUDWidget::UpdatePlayerState()
         {
            ABCHECK(CurrentPlayerState.IsValid());

            PlayerName->SetText(FText::FromString(CurrentPlayerState->GetPlayerName()));
            PlayerLevel->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetCharacterLevel())));
            CurrentScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameScore())));
         }
         ```

         </details>

   2. 컨트롤러에서 HUD 위젯과 플레이어 스테이트를 연결하고 캐릭터에서는 HUD위젯과 캐릭터 스텟 컴포넌트를 연결한다.
      - 2시간 애먹었다.  
         <details><summary>코드 보기</summary>

         ```c++
         //ABPlayerController.cpp
         void AABPlayerController::BeginPlay()
         {
            ...
            //연결
            auto ABPlayerState = Cast<AABPlayerState>(PlayerState);
            ABCHECK(nullptr != ABPlayerState);
            HUDWidget->BindPlayerState(ABPlayerState);
            ABPlayerState->OnplayerStateChanged.Broadcast();	//알린다.
         }
         //ABCharacter.cpp
         #include "ABHUDWidget.h"
         void AABCharacter::SetCharacterState(ECharacterState NewState)
         {
            ...
            switch (CurrentState)
            {
            case ECharacterState::LOADING:
               if(bIsPlayer) 
               {
                  DisableInput(ABPlayerController);

                  ABPlayerController->GetHUDWidget()->BindCharcterStat(CharacterStat);
                  ...
               }
               ...
            }
            ...
         }
         ```

         </details>

> **<h3>Realization</h3>**
- 게임 데이터는 액터에서 보관한다.

## **06.10**
> **<h3>Today Dev Story</h3>**
- ### 경험치 정보 추가
   - <img src="Image/Exp_Up.gif" height="300" title="Exp_Up"> 
   - NPC가 사망하면 NPC의 레벨에 지정된 경험칠를 플레이어에게 전달하고 레벨업 하는 기능 구현
      - UI Progress로 표현하기에 비율 정보 계산 필요. 캐릭터 스텟에 NPC를 위한 경험치 값 설정. 플레이어 스테이트에 플레이어 경험치 보관
      - 캐릭터 사망시 NPC가 플레이어에게 죽는지 검사하고 플레이어 컨트롤러를 통해 플레이어 스테이드 업데이트
      - 데미지 프레임워크에서 컨트롤러의 정보를 가해자 인자로 전달되므로 이를 사용한다.
      
      <details><summary>코드 보기</summary>

      ```c++
      //ABPlayerState.h
      public:
      	float GetExpRatio() const;
	      bool AddExp(int32 IncomeExp);
         
      protected:
         UPROPERTY(Transient)
         int32 Exp;

      private:
         void SetCharacterLevel(int32 NewCharacterLevel);

         struct FABCharacterData* CurrentStatData;
      //ABPlayerState.cpp  
      #include "ABGameInstance.h"
      ...
      float AABPlayerState::GetExpRatio() const
      {
         if(CurrentStatData->NextExp <= KINDA_SMALL_NUMBER) return 0.0f;

         float Result = (float)Exp/(float)CurrentStatData->NextExp;
         ABLOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"),Result,Exp,CurrentStatData->NextExp);
         return Result;
      }

      bool AABPlayerState::AddExp(int32 IncomeExp)
      {
         if(CurrentStatData->NextExp == -1) return false; //만랩

         bool DidLevelUp = false;
         Exp += IncomeExp;
         if(Exp >= CurrentStatData->NextExp)
         {
            Exp -= CurrentStatData->NextExp;
            SetCharacterLevel(CharacterLevel + 1);
            DidLevelUp = true;
         }
         OnPlayerStateChanged.Broadcast();	//스텟바뀌었다!
         return DidLevelUp;
      }

      void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)	//레벨업
      {
         auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
         ABCHECK(nullptr != ABGameInstance);

         CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel);
         ABCHECK(nullptr != CurrentStatData);
         
         CharacterLevel = NewCharacterLevel;
      }
      //ABHUDWidget.cpp
      void UABHUDWidget::UpdatePlayerState()
      {
         ABCHECK(CurrentPlayerState.IsValid());

         ExpBar->SetPercent(CurrentPlayerState->GetExpRatio());
         ...
      }
      //ABCharacterStatComponent.h
      public:
      	int32 GetDropExp() const;
      //ABCharacterStatComponent.cpp
      int32 UABCharacterStatComponent::GetDropExp() const
      {         
         return CurrentStatData->DropExp;
      }
      //ABCharacter.h
      public:
         int32 GetExp() const;
      //ABCharacter.cpp
      int32 AABCharacter::GetExp() const	//NPC인 경우에만 해당
      {
         return CharacterStat->GetDropExp();
      }
      ...
      float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
      {
         ...
         if(CurrentState == ECharacterState::DEAD)
         {
            if(EventInstigator->IsPlayerController())
            {
               ABPlayerController = Cast<AABPlayerController>(EventInstigator);
               ABCHECK(nullptr != ABPlayerController,0.0f);
               ABPlayerController->NPCKill(this);
            }
         }
         ...
      }
      //ABPlayerController.h
      public:
      	void NPCKill(class AABCharacter* KilledNPC) const;
      private:
         UPROPERTY()
	      class AABPlayerState* ABPlayerState;
      //ABPlayerController.cpp
      void AABPlayerController::BeginPlay()
      {
         ...
         ABPlayerState = Cast<AABPlayerState>(PlayerState);
         ...
      }
      ...
      void AABPlayerController::NPCKill(AABCharacter* KilledNPC) const
      {
         ABPlayerState->AddExp(KilledNPC->GetExp());
      }
      ```

      </details>

- ### 게임 데이터의 관리
   - <img src="Image/Score_UP.gif" height="300" title="Score_UP">
   - UI오른쪽 상단에 게임의 스코어를 구현 NPC를 잡을때마다 상승하며 게임 데이터만 관리하는 GameStateBase를 부모로한 ABGameState 클래스를 생성한다.
      - 게임 스코어를 저장하는 속성 추가

      <details><summary>코드 보기</summary>

      ```c++
      //ABGameState.h
      class ARENA_API AABGameState : public AGameStateBase
      {
         GENERATED_BODY()

      public:
         AABGameState();

         int32 GetTotalGameScore() const;
         void AddGameScore();

      private:
         UPROPERTY(Transient)
         int32 TotalGameScore;
      };
      //ABGameState.cpp
      AABGameState::AABGameState()
      {
         TotalGameScore = 0;
      }

      int32 AABGameState::GetTotalGameScore() const
      {
         return TotalGameScore;
      }

      void AABGameState::AddGameScore()
      {
         TotalGameScore++;
      }
      //ABGameMode.cpp
      #include "ABGameState.h"
      AABGameMode::AABGameMode()
      {
         ...
         GameStateClass = AABGameState::StaticClass();
      }
      ```

      </details>

   - NPC가 제거되면 섹션 액터의 스테이트를 COMPLETE로 변경하고 마지막으로 데미지를 입힌 컨트롤러의 기록을 LastHitBy속성을 통해 정보를 가져온다.
      - 이 방식의 경우 소멸될 때 검사를 하기에 Instigator보다 효율적이다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABSection.h
      private:
      	UFUNCTION()
	      void OnKeyNPCDestroyed(AActor* DestroyActor);
      //ABSection.cpp
      #include "ABPlayerController.h"
      ...
      void AABSection::OnNPCSpawn()
      {
         GetWorld()->GetTimerManager().ClearTimer(SpawnNPCTimerHandle);
         auto KeyNPC = GetWorld()->SpawnActor<AABCharacter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator);
         if(nullptr != KeyNPC) KeyNPC->OnDestroyed.AddDynamic(this, &AABSection::OnKeyNPCDestroyed);
      }
      void AABSection::OnKeyNPCDestroyed(AActor* DestroyActor)
      {
         auto ABCharacter = Cast<AABCharacter>(DestroyActor);
         ABCHECK(nullptr != ABCharacter);

         auto ABPlayerController = Cast<AABPlayerController>(ABCharacter->LastHitBy);
         ABCHECK(nullptr != ABPlayerController);

         SetState(EsectionState::COMPLETE);
      }
      ```

      </details>

   - NPC를 제거해 섹션을 클리어하면 게임 모드에게 스코어를 올리라는 명령을 내린다.
      - 마지막 피격 컨트롤러 정보를 넘기고 해당 플레이어 스테이트 스코어를 높이고 전체 스코어도 높인다.
      - 현재 참여중인 컨트롤러의 목록은 월드에서 제공하는 GetPlayerControllerIterator를 사용한다.
   
      <details><summary>코드 보기</summary>

      ```c++
      //ABPlayerState.h
      public:
      	void AddGameScore();
      //ABPlayerState.cpp
      void AABPlayerState::AddGameScore()
      {
         GameScore++;
         OnPlayerStateChanged.Broadcast();
      }
      //ABPlayerController.h
      public:
      	void AddGameScore() const;
      //ABPlayerController.cpp
      void AABPlayerController::AddGameScore() const
      {
         ABPlayerState->AddGameScore();	
      }
      //ABGameMode.h
      class ARENA_API AABGameMode : public AGameModeBase
      {
         GENERATED_BODY()
      public:
         AABGameMode();

         virtual void PostInitializeComponents() override;
         virtual void PostLogin(APlayerController* NewPlayer) override;
         void AddScore(class AABPlayerController* ScoredPlayer);

      private:
         UPROPERTY()
         class AABGameState* ABGameState;
      };
      //ABGameMode.cpp
      void AABGameMode::PostInitializeComponents()
      {
         Super::PostInitializeComponents();
         ABGameState = Cast<AABGameState>(GameState);
      }

      void AABGameMode::AddScore(AABPlayerController* ScoredPlayer)
      {
         for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
         {
            const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
            if((nullptr != ABPlayerController) & (ScoredPlayer == ABPlayerController))
            {
               ABPlayerController->AddGameScore();
               break;
            }
         }
         ABGameState->AddGameScore();
      }
      //ABSection.cpp
      void AABSection::OnKeyNPCDestroyed(AActor* DestroyActor)
      {
         ...
         auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
         ABCHECK(nullptr != ABGameMode);
         ABGameMode->AddScore(ABPlayerController);
         ...
      }
      ```

      </details>

- ### 게임 데이터의 저장과 로딩 
   - <img src="Image/Data_Save.gif" height="300" title="Data_Save">
   - SaveGame이라는 부모를 상속받아 게임데이터의 저장과 로딩을 간편하기 구현할 수 있다. (ABSaveGame)
   - 각 플랫폼별로 최적의 장소에 데이터가 저장되며, 게임 데이터를 저장하는 경우 프로젝츠의 saved 폴더에 있는 savegames라는 폴더에 저장된다.
      - 각 저장파일에 접근할 수 있는 고유 이름인 슬롯 이름이 필요하다. Player1이라는 슬롯 이름을 사용해 하나의 세이브 파일만 관리한다.
      - 처음에는 세이브된 데이터가 없기 때문에 PlayerState에서 InitPlayerDtat에 기본 세이브 데이터를 생성하는 로직을 구현한다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABSaveGame.h
      class ARENA_API UABSaveGame : public USaveGame
      {
         GENERATED_BODY()
      public:
         UABSaveGame();

         UPROPERTY()
         int32 Level;

         UPROPERTY()
         int32 Exp;

         UPROPERTY()
         FString PlayerName;

         UPROPERTY()
         int32 HighScore;
      };
      //ABSaveGame.cpp
      UABSaveGame::UABSaveGame()
      {
         Level = 1;
         Exp = 0;
         PlayerName = TEXT("Guest");
         HighScore = 0;
      }
      //ABPlayerState.cpp
      #include "ABSaveGame.h"
      AABPlayerState::AABPlayerState()
      {
         CharacterLevel = 1;
         GameScore = 0;
         GameHighScore = 0;
         Exp = 0;
         SaveSlotName = TEXT("Player1");
      }
      ...
      int32 AABPlayerState::GetGameHighScore() const
      {
         return GameHighScore;
      }
      void AABPlayerState::InitPlayerData()	//초기 생성
      {
         auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName,0));
         if(nullptr == ABSaveGame) ABSaveGame = GetMutableDefault<UABSaveGame>();	//없다면 저장 데이터를 새로만든다.

         //데이터를 불러온다.
         SetPlayerName(ABSaveGame->PlayerName);
         SetCharacterLevel(ABSaveGame->Level);
         GameScore = 0;
         GameHighScore = ABSaveGame->HighScore;
         Exp = ABSaveGame->Exp;
      }
      ...
      void AABPlayerState::AddGameScore() //최고기록갱신
      {
         GameScore++;
         if(GameScore >= GameHighScore) GameHighScore = GameScore;
         OnPlayerStateChanged.Broadcast();
      }
      ```

      </details>
   
   - 플레이어에 관련된 데이터가 변경될 때마다 이를 저장하도록 구현한다. 최초에 데이터를 저장하고 경험치에 변동이 있을 때마다 저장한다.
      - AddGameScore와 AddExp에 SavePlayerData()힘수를 추가한다.
      - 플레이어 스테이트의 HighScore 값을 HUD UI에 연동한다.
   
      <details><summary>코드 보기</summary>

      ```c++
      //ABPlayerState.h
      public:
      	void SavePlayerData();
      //ABPlayerState.cpp
      void AABPlayerState::InitPlayerData()	//초기 생성
      {
         ...
         SavePlayerData();
      }
      void AABPlayerState::SavePlayerData()
      {
         UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
         NewPlayerData->PlayerName = GetPlayerName();
         NewPlayerData->Level = CharacterLevel;
         NewPlayerData->Exp = Exp;
         NewPlayerData->HighScore = GameHighScore;

         if(!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName,0)) ABLOG(Error, TEXT("SaveGame Error!"));
      }
      ...
      //AddGameScore와 AddExp에 	SavePlayerData(); 추가
      //ABHUDWidget.cpp
      void UABHUDWidget::UpdatePlayerState()
      {
         ...
         HighScore->SetText(FText::FromString(FString::FromInt(CurrentPlayerState->GetGameHighScore())));
      }
      ```

      </details>

- ### 전투 시스템의 설계
   - 난이도를 점진적으로 높이기 위해 부가 요소를 추가한다.
   1. 캐릭터는 무기를 들때 더 긴 공격범위를 가진다.
      - <img src="Image/Attack_Range_Set.gif" height="300" title="Attack_Range_Set">
      - 무기 액터임 ABWeapon에 AttackRange라는 속성을 추가하고, 무기가 없을 때는 캐릭터의 AttackRange를 사용한다.
         - 캐릭터의 AttackRange 기본값을 80으로 낮추고 무기는 150으로 지정한다. 
         - 해당 속성 키워드에 EditAnywhere, Blueprint, ReadWrite를 지정해 ABWeapon 클래스를 상속받은 무기 블루프린트에서도 공격범위를 다르게 설정할 수 있도록 기능을 부여한다.
         - 또한 무기를 변경할 수 있도록 CanSetWeapon의 값을 무조건 true로 설정하고 기존 무기가 있어도 새로운 무기를 습득하도록 변경한다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABWeapon.h
         public:
            float GetAttackRange() const;
         private:
            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Attack)
            float AttackRange;
         //ABWeapon.cpp
         AABWeapon::AABWeapon()
         {
            ... 
            AttackRange = 150.0f;
         }
         float AABWeapon::GetAttackRange() const
         {
            return AttackRange;
         }
         //ABCharacter.h
         public:
            float GetFinalAttackRange() const;
         //ABCharacter.cpp
         AABCharacter::AABCharacter()
         {
            ...
            AttackRange = 80.0f;
            ...
         }
         ...
         float AABCharacter::GetFinalAttackRange() const	//무기의 여부에 따라 거리 차이
         {
            return (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackRange() : AttackRange;
         }
         bool AABCharacter::CanSetWeapon()	//무기 장착 가능 여부
         {
            return true;
         }
         void AABCharacter::SetWeapon(AABWeapon* NewWeapon)	//무기를 장착시키는 로직
         {
            ABCHECK(nullptr != NewWeapon);

            if(nullptr != CurrentWeapon)	//기존 무기가 있을때 파괴한다.
            {
               CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
               CurrentWeapon->Destroy();
               CurrentWeapon = nullptr;
            }
            ...
         }
         ...
         void AABCharacter::AttackCheck()
         {
            float FinalAttackRange = GetFinalAttackRange();	//범위를 거져온다.
            
            ...
               GetActorLocation() + GetActorForwardVector() * FinalAttackRange,
            ...
            
         #if ENABLE_DRAW_DEBUG
            FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
            FVector Center = GetActorLocation() + TraceVec * 0.5f;
            float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
            ...
         }
         //BTDecorator_IsInAttackRange.cpp
         bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
         {
            ...
            auto ControllingPawn = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
            ...
            bResult = (Target->GetDistanceTo(ControllingPawn) <= ControllingPawn->GetFinalAttackRange());
            return bResult;
         }
         ```

         </details>
   
   2. 공격할 때 무기가 있는 경우 기존 공격력을 증폭시킨다.
      - <img src="Image/Damage_Set.png" height="250" title="Damage_Set">
      - 무기에는 공격력 증가치가 랜덤으로 부여되며, 운이 없으면 오히려 무기에 의해 공격력이 저하될 수 있다.
      - 무기에 공격력과 효과치 속성을 설정하고 최종 대미지를 산출할 때 이 데이터를 활용한다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABWeapon.h
         public:	
            ...
            float GetAttackDamage() const;
            float GetAttackModifier() const;

         protected:
            ...
            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Attack)
            float AttackDamageMin;
            
            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Attack)
            float AttackDamageMax;
            
            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Attack)
            float AttackModifierMin;
            
            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Attack)
            float AttackModifierMax;
            
            UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category=Attack)
            float AttackDamage;

            UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category=Attack)
            float AttackModifier;
         //ABWeapon.cpp
         AABWeapon::AABWeapon()
         {
            AttackRange = 150.0f;
            AttackDamageMin = -2.5f;
            AttackDamageMax = 10.0f;
            AttackModifierMin = 0.85f;
            AttackModifierMax = 1.25f;
         }

         float AABWeapon::GetAttackDamage() const
         {
            return AttackDamage;
         }

         float AABWeapon::GetAttackModifier() const
         {
            return AttackModifier;
         }

         void AABWeapon::BeginPlay()
         {
            Super::BeginPlay();

            AttackDamage = FMath::RandRange(AttackDamageMin,AttackDamageMax);
            AttackModifier = FMath::RandRange(AttackModifierMin,AttackModifierMax);
            ABLOG(Warning,TEXT("Weapon Damage : %f, Modifier : %f"),AttackDamage,AttackModifier);
         }
         //ABCharacter.h
         public: 
            float GetFinalAttackDamage() const;
         //ABCharacter.cpp
         float AABCharacter::GetFinalAttackDamage() const
         {
            float AttackDamage = (nullptr != CurrentWeapon) ? (CharacterStat->GetAttack() + CurrentWeapon->GetAttackDamage()) : CharacterStat->GetAttack();
            float AttackModifer = (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackModifier() : 1.0f;
            return AttackDamage * AttackModifer;
         }
         void AABCharacter::AttackCheck()
         {
            ...
         #endif

            if(bReslut)
            {
               if(HitResult.Actor.IsValid())
               {
                  ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());

                  FDamageEvent DamageEnvent;
                  HitResult.Actor->TakeDamage(GetFinalAttackDamage(), DamageEnvent, GetController(), this);
               }
            }
         }
         ```

         </details>

   3. 현재 게임 스코어가 높을수록 생성되는 NPC의 레벨도 증가한다.
      - <img src="Image/NPC_Level.png" height="250" title="NPC_Level">
      - NPC 캐릭터의 Loading 스테이트에서 현재 게임 스코어를 게임 모드에 질의하고 캐릭터의 레벨 값을 설정

         <details><summary>코드 보기</summary>

         ```c++
         //ABGameMode.h
         public:
            int32 GetScore() const;
         //ABGameMode.cpp
         int32 AABGameMode::GetScore() const
         {
            return ABGameState->GetTotalGameScore();
         }
         //ABCharacter.cpp
         void AABCharacter::SetCharacterState(ECharacterState NewState)
         {
            ...
            switch (CurrentState)
            {
            case ECharacterState::LOADING:
               ...
               else
               {
                  auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
                  ABCHECK(nullptr != ABGameMode);
                  int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f));
                  int32 FinalLevel = FMath::Clamp<int32>(TargetLevel,1,20);
                  ABLOG(Warning,TEXT("New NPC Level : %d"),FinalLevel);
                  CharacterStat->SetNewLevel(FinalLevel);		//NPC의 레벨을 바꾼다.
               }
               ...
            }
            ...
         }
         ```

         </details>

- ### 타이틀 화면의 제작_메인화면
   - <img src="Image/Main_Menu.png" height="250" title="Main_Menu">
   - 공백 레벨을 만들고 Title이라는 이름으로 저장한다. 해당 레벨은 기능없이 UI만 띄워준다.
   - PlayerController를 부모로 하는 ABUIPlayerController라는 이름의 클래스를 만든다.
      - 이를 상속받은 블루프린트에서 UI클래스 값을 띄울수 있도록 EditDefaultOnly 키워드를 지정한다.
      - 게임을 시작하면 해당 클래스로부터 UI인스턴스를 생성하고 뷰포트에 띄운 후 입력은 UI에만 전달되도록 한다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABUIPlayerController.h
      class ARENA_API AABUIPlayerController : public APlayerController
      {
         GENERATED_BODY()

      public:
         virtual void BeginPlay() override;

         UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=UI)
         TSubclassOf<class UUserWidget> UIWidgetClass;
         
         UPROPERTY()
         class UUserWidget* UIWidgetInstance;
      };
      //ABUIPlayerController.cpp
      #include "ABUIPlayerController.h"
      #include "Blueprint/UserWidget.h"

      void AABUIPlayerController::BeginPlay()
      {
         Super::BeginPlay();

         ABCHECK(nullptr != UIWidgetClass);

         UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
         ABCHECK(nullptr != UIWidgetInstance);

         //뷰포트에 띄운다.
         UIWidgetInstance->AddToViewport();

         //UI에만 입력가능
         FInputModeUIOnly Mode;
         Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
         SetInputMode(Mode);
         bShowMouseCursor = true;
      }
      ```

      </details>

   - 위의 cpp파일을 기반으로 하는 BP_TitleUIPlayerController라는 블루프린트를 생성한다.
      - UIWidget Class를 UI_Title(미리 가져온)으로 설정한다.
   - BP_TitleGameMode라는 블루프린트를 생성하여 BP_TitleUIPlayerController를 띄워준다. (c++가 아닌 블루프린트로)
      - Default Pawn Class를 아무런 기능이 없는 Pawn, Player Controller Class를 BP_TitleUIPlayerController로 설정한다.

   - <img src="Image/UI_Title.png" height="250" title="UI_Title">
   - UI에셋 내에는 로직이 존재해 새로 시작하기를 누르면 캐릭터 선택을 위한 select로 이동하고, 이어하기는 GamePlay 레벨로 이동한다.
      - 또한 이어하기할때 데이터가 없다면 비활성화 된다.

- ### 타이틀 화면의 제작_선택창

   1. 캐릭터 선택창의 구현
      - <img src="Image/Select_Menu.png" height="250" title="Select_Menu">
      - ABUIPlayerController를 상속받은 BP_SelectUIPlayerController 블루프린트를 생성하고 UIWidget Class 속성을 UI_Select로 지정한다.
      - BP_SelectGameMode라는 게임 모드를 생성하고 PlayerController 클래스와 DefaultPawnClass를 각각 BP_SelectUIPlayerController와 Pawn으로 지정한다.
      - 월드 셋팅에서 BP_SelectGameMode를 기본 게임 모드로 지정하면 양옆으로 버튼이 보인다.

   2. 좌우 버튼의 로직을 구현한다.
      - <img src="Image/Select_Character.gif" height="300" title="Select_Character">
      - UserWidget을 상속받은 ABCharacterSelectwidget 클래스를 생성한다.
         - 스켈레탈 메시 액터의 목록을 가져오고 버튼을 누를 때마다 스켈레탈 메시를 변경해준다.
         - 특정 타입을 상속받은 액터의 목록은 TActorIterator<액터타입>구문을 사용해 가져온다.
      - NextCharacter 함수는 블루프린트에서 사용할 수 있도록 함수에 UFUNCTION 매크로와 BlueprintCallable 키워드를 추가한다.
      - 컴파일 후 UI_Select에셋의 부모 클래스를 ABCharacterSelectWidget으로 변경한다.
   
         <details><summary>코드 보기</summary>

         ```c++
         //ABCharacterSeletWidget.h
         class ARENA_API UABCharacterSelectWidget : public UUserWidget
         {
            GENERATED_BODY()

         protected:
            UFUNCTION(BlueprintCallable)
            void NextCharacter(bool bForward = true);

            virtual void NativeConstruct() override;

         protected:
            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Character)
            int32 CurrentIndex;

            UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Character)
            int32 MaxIndex;

            TWeakObjectPtr<USkeletalMeshComponent> TargetComponent;
         
            UPROPERTY()
            class UButton* PreButton;

            UPROPERTY()
            class UButton* NextButton;

            UPROPERTY()
            class UEditableTextBox* TextBox;

            UPROPERTY()
            class UButton* ConfirmButton;

         private:
            UFUNCTION()
            void OnPrevClicked();

            UFUNCTION()
            void OnNextClicked();

            UFUNCTION()
            void OnConfirmClickecd();
         };
         //ABCharacterSeletWidget.cpp
         #include "ABCharacterSelectWidget.h"
         #include "ABCharacterSetting.h"
         #include "ABGameInstance.h"
         #include "EngineUtils.h"
         #include "Animation/SkeletalMeshActor.h"
         #include "Components/Button.h"
         #include "Components/EditableTextBox.h"
         #include "ABSaveGame.h"
         #include "ABPlayerState.h"

         void UABCharacterSelectWidget::NextCharacter(bool bForward)
         {
            bForward ? CurrentIndex++ : CurrentIndex--;

            if(CurrentIndex == -1) CurrentIndex = MaxIndex - 1;
            if(CurrentIndex == MaxIndex) CurrentIndex = 0;

            auto CharacterSetting = GetDefault<UABCharacterSetting>();
            auto AssetRef = CharacterSetting->CharacterAssets[CurrentIndex];	//에셋을 불러옴

            auto ABGameInstance = GetWorld()->GetGameInstance<UABGameInstance>();
            ABCHECK(nullptr != ABGameInstance);
            ABCHECK(TargetComponent.IsValid());

            USkeletalMesh* Asset = ABGameInstance->StreamableManager.LoadSynchronous<USkeletalMesh>(AssetRef);	//생성
            if(nullptr != Asset) TargetComponent->SetSkeletalMesh(Asset);
         }

         void UABCharacterSelectWidget::NativeConstruct()	// UI가 뷰포트에 추가되면 호출되는 함수 (초기화)
         {
            Super::NativeConstruct();

            CurrentIndex = 0;
            auto CharacterSetting = GetDefault<UABCharacterSetting>();
            MaxIndex = CharacterSetting->CharacterAssets.Num();

            for(TActorIterator<ASkeletalMeshActor> It(GetWorld());It;++It)
            {
               TargetComponent = It->GetSkeletalMeshComponent();
               break;
            }

            PrevButton = Cast<UButton>(GetWidgetFromName(TEXT("btnPrev")));
            ABCHECK(nullptr != PrevButton);
            
            NextButton = Cast<UButton>(GetWidgetFromName(TEXT("btnNext")));
            ABCHECK(nullptr != NextButton);

            TextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("edtPlayerName")));
            ABCHECK(nullptr != TextBox);

            ConfirmButton = Cast<UButton>(GetWidgetFromName(TEXT("btnConfirm")));
            ABCHECK(nullptr != ConfirmButton);

            PrevButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnPrevClicked);
            NextButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnNextClicked);
            ConfirmButton->OnClicked.AddDynamic(this, &UABCharacterSelectWidget::OnConfirmClicked);
         }

         void UABCharacterSelectWidget::OnPrevClicked()
         {
            NextCharacter(false);
         }

         void UABCharacterSelectWidget::OnNextClicked()
         {
            NextCharacter(true);
         }

         void UABCharacterSelectWidget::OnConfirmClicked()
         {
            FString CharacterName = TextBox->GetText().ToString();
            if(CharacterName.Len() <= 0 || CharacterName.Len() > 10) return;
            
            UABSaveGame* NewPlayerData = NewObject<UABSaveGame>();
            NewPlayerData->PlayerName = CharacterName;
            NewPlayerData->Level = 1;
            NewPlayerData->Exp = 0;
            NewPlayerData->HighScore = 0;

            auto ABPlayerState = GetDefault<AABPlayerState>();
            if (UGameplayStatics::SaveGameToSlot(NewPlayerData, ABPlayerState->SaveSlotName, 0)) UGameplayStatics::OpenLevel(GetWorld(), TEXT("Gameplay"));
            else ABLOG(Error, TEXT("SaveGame Error!"));
         }
         ```

         </details>

> **<h3>Realization</h3>**
- 게임 데이터의 저장은 SaveGame에서 관리한다. 
   - 각 플랫폼별로 최적의 장소에 데이터가 저장되며, 게임 데이터를 저장하는 경우 프로젝츠의 saved 폴더에 있는 savegames라는 폴더에 저장된다.
- 언리얼 오브젝트를 생성할때 NewObject 명령을 사용하며 더 이상 사용하지 않으면 알아서 가비지 컬렉터가 탐지해 자동으로 소멸시킨다.
- 캐릭터 선택창은 새로운 Map으로 저장

## **06.11**
> **<h3>Today Dev Story</h3>**
- ### 타이틀 화면의 제작_선택창_2
   - <img src="Image/Load_To_Continue.gif" height="300" title="Load_To_Continue">
   - 현재 선택한 캐릭터가 게임 플레이에서도 동일하게 나오도록 선택한 캐릭터의 정보를 저장하고 게임플레이 레벨에서 이를 로딩하는 기능을 만든다.
   - 생성하기 버튼을 누르면 현재 선택한 캐릭터 정보를 세이브 데이터에 저장하고 이를 읽어들이는 기능을 구현한다.
   
      <details><summary>코드 보기</summary>

      ```c++
      //ABSaveGame.h
      public:
      	UPROPERTY()
	      int32 CharacterIndex;
      //ABSaveGame.cpp
      CharacterIndex = 0;
      //ABCharacterSelectWidget.cpp
      void UABCharacterSelectWidget::OnConfirmClicked()
      {
         ...
         NewPlayerData->CharacterIndex = CurrentIndex;
         ...
      }
      //ABPlayerState.h
      public:
      	int32 GetCharacterIndex() const;
      protected:
      	UPROPERTY(Transient)
         int32 CharacterIndex;
      //ABPlayerState.cpp
      AABPlayerState::AABPlayerState()
      {
         ...
         CharacterIndex = 0;
      }
      int32 AABPlayerState::GetCharacterIndex() const
      {
         return CharacterIndex;
      }
      void AABPlayerState::InitPlayerData()	//초기 생성
      {
         ...
         CharacterIndex = ABSaveGame->CharacterIndex;
         SavePlayerData();
      }
      void AABPlayerState::SavePlayerData()
      {
         ...
         NewPlayerData->CharacterIndex = CharacterIndex;

         if(!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName,0)) ABLOG(Error, TEXT("SaveGame Error!"));
      }
      //ABCharacter.cpp
      void AABCharacter::BeginPlay()
      {
         ...
         auto DefaultSetting = GetDefault<UABCharacterSetting>();
         
         if(bIsPlayer)	//플레이어인 경우
         {
            auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
            ABCHECK(nullptr != ABPlayerState);
            AssetIndex = ABPlayerState->GetCharacterIndex();
         }
         ...
      }
      ```

      </details>

> **<h3>Realization</h3>**
- null

## **06.12**
> **<h3>Today Dev Story</h3>**
- ### 게임의 중지화면
   1. 위젯의 기본 뼈대 만들기
      - 버튼은 기능별로 동일한 이름을 가지도록 설계
         - btnResume, btnReturnToTitle, btnRetryGame
         - 프로젝트 세팅 > 입력에서 GamePause라는 이름의 액션 매핑을 M키를 사용해 게임플레이의 종료를 구현한다.
         - 플레이어 컨트롤러에서 해당 키를 인식시키는 코드를 추가한다.
      - 또한 UI가 공용으로 사용할 UserWidget을 상속받은 기본 클래스(ABGameplayWidget)를 만들고 이를 UI_Menu 에셋에서의 부모 클래스가 된다.
         - UI를 초기화하는 시점에 발생하는 NativeConstruct함수에서 이름으로 버튼을 찾고 존재하면 바인딩한다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABPlayerController.h
         protected:
            virtual void SetupInputComponent() override;
         private:
            void OnGamePause();
         //ABPlayerController.cpp
         void AABPlayerController::SetupInputComponent()
         {
            Super::SetupInputComponent();
            InputComponent->BindAction(TEXT("GamePause"),EInputEvent::IE_Pressed, this, &AABPlayerController::OnGamePause);	//눌리는지 확인
         }

         void AABPlayerController::OnGamePause() { }
         //ABGameplayWidget.h
         class ARENA_API UABGameplayWidget : public UUserWidget
         {
            GENERATED_BODY()

         protected:
            virtual void NativeConstruct() override; //UI위젯이 초기화되는 시점 -> 이름으로 버튼 찾을거

            UFUNCTION()
            void OnResumeClicked();

            UFUNCTION()
            void OnReturnToTitleClicked();

            UFUNCTION()
            void OnRetryGameClicked();

            UPROPERTY()
            class UButton* ResumeButton;

            UPROPERTY()
            class UButton* ReturnToTitleButton;
            
            UPROPERTY()
            class UButton* RetryGameButton;
         };
         //ABGameplayWidget.cpp
         #include "ABGameplayWidget.h"
         #include "Components/Button.h"

         void UABGameplayWidget::NativeConstruct()
         {
            Super::NativeConstruct();

            ResumeButton = Cast<UButton>(GetWidgetFromName(TEXT("btnResume")));	//찾아라
            if(nullptr != ResumeButton) ResumeButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnResumeClicked); //바인딩

            ReturnToTitleButton = Cast<UButton>(GetWidgetFromName(TEXT("btnReturnToTitle")));
            if(nullptr != ReturnToTitleButton) ReturnToTitleButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnReturnToTitleClicked);

            RetryGameButton = Cast<UButton>(GetWidgetFromName(TEXT("btnRetryGame")));
            if(nullptr != RetryGameButton) RetryGameButton->OnClicked.AddDynamic(this, &UABGameplayWidget::OnRetryGameClicked);
         }

         void UABGameplayWidget::OnResumeClicked() { }

         void UABGameplayWidget::OnReturnToTitleClicked() { }

         void UABGameplayWidget::OnRetryGameClicked() { }
         ```

         </details>
   
   2. M 버튼을 누르면 메뉴 UI가 나타나도록 추가한다.
      - <img src="Image/Pause_Menu.gif" height="300" title="Pause_Menu">
      - 게임 중지, 마우스커서 출력, 입력이 UI에만 전달되도록 제어해야한다.
         - PlayerController의 SetPause 함수를 사용하면 플레이를 일시 중지할 수 있다.
         - 마우스 커서도 bShowMouseCursor 속성을 true로 하면 보여지고 SetInputMode에 FinputModeUIOnly 클래스를 넣어 UI에만 입력되도록한다.
         - RemoveFromParent함수를 통해 뷰포트의 자신을 제거하고 GetOwningPlayer함수를 통해 자신을 관리하는 플레이어 컨트롤러의 정보를 가져온다. 이를 사용해 게임의 진행을 원래되로 되돌려놓는다.

         <details><summary>코드 보기</summary>

         ```c++
         //PlayerController.h
         public:
            void ChangeInputMode(bool bGameMode = true);
         protected:
            UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=UI)
            TSubclassOf<class UABGameplayWidget> MenuWidgetClass;
         private:
            UPROPERTY()
            class UABGameplayWidget* MenuWidget;

            FInputModeGameOnly GameInputMode;
            FInputModeUIOnly UIInputMode;
         //PlayerController.cpp
         #include "ABGameplayWidget.h"
         AABPlayerController::AABPlayerController()
         {
            ...
            static ConstructorHelpers::FClassFinder<UABGameplayWidget>
            UI_MENU_C(TEXT("/Game/Book/UI/UI_Menu.UI_Menu_C"));
            if(UI_MENU_C.Succeeded()) MenuWidgetClass = UI_MENU_C.Class;
         }
         void AABPlayerController::BeginPlay()
         {
            Super::BeginPlay();
            ChangeInputMode(true);	//입력모드를 싸움
            ...
         }
         ...
         void AABPlayerController::ChangeInputMode(bool bGameMode)
         {
            if(bGameMode)	//게임에 입력
            {
               SetInputMode(GameInputMode);	
               bShowMouseCursor = false;
            }
            else
            {
               SetInputMode(UIInputMode);
               bShowMouseCursor = true;	//마우스 커서 띄움
            }
         }

         void AABPlayerController::OnGamePause()
         {
            MenuWidget = CreateWidget<UABGameplayWidget>(this, MenuWidgetClass);	//만들어
            ABCHECK(nullptr != MenuWidget);
            MenuWidget->AddToViewport(3); //화면에 추가
            
            SetPause(true);
            ChangeInputMode(false);	//입력모드 바꿈
         }
         //ABGameplayWidget.cpp
         #include "ABPlayerController.h"
         ...
         void UABGameplayWidget::OnResumeClicked()
         {
            auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer());
            ABCHECK(nullptr != ABPlayerController);

            //UI제거하고 이어하기
            RemoveFromParent();	
            ABPlayerController->ChangeInputMode(true);
            ABPlayerController->SetPause(false);
         }

         void UABGameplayWidget::OnReturnToTitleClicked()
         {
            UGameplayStatics::OpenLevel(GetWorld(),TEXT("Title"));
         }
         ```

         </details>

- ### 게임의 결과화면
- <img src="Image/Game_End.gif" height="300" title="Game_End">

   1. 결과화면 UI인 ABGameplayeWidget을 상속받은 ABGameplayResultWidget 클래스를 만든다.
      - 결과화면에는 다시시작하는 버튼과 결과를 보여주는 텍스트 점수를 보여주는 3개의 위젯컨트롤리 있다.
      - UI_Result 에셋의 부모 클래스로 설정하고 이 UI는 게임 종료시 한번만 띄워지는 로직을 구현한다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABGameplayResultWidget.h
         protected:
            void NativeConstruct() override;
         //ABGameplayResultWidget.cpp
         #include "Components/TextBlock.h"
         //button은 ABGameplayResultWidget에서 이미 상속받았음

         void UABGameplayResultWidget::NativeConstruct()
         {
            Super::NativeConstruct();

            auto Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult")));
            ABCHECK(nullptr != Result);

            auto TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore")));
            ABCHECK(nullptr != TotalScore);
         }
         //ABPlayerController.h
         public:
            void ShowResultUI();
         protected:
            UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=UI)
            TSubclassOf<class UABGameplayResultWidget> ReultWidgetClass;
         private:
            UPROPERTY()
            class UABGameplayResultWidget* ReultWidget;
         //ABPlayerController.cpp
         AABPlayerController::AABPlayerController()
         {
            ...
            static ConstructorHelpers::FClassFinder<UABGameplayResultWidget>
            UI_RESULT_C(TEXT("/Game/Book/UI/UI_Result.UI_Result_C"));
            if(UI_RESULT_C.Succeeded()) ResultWidgetClass = UI_RESULT_C.Class;
         }
         ...
         void AABPlayerController::BeginPlay()
         {
            ...
            ResultWidget = CreateWidget<UABGameplayResultWidget>(this, ResultWidgetClass);
            ABCHECK(nullptr != ResultWidget);
            ...
         }
         void AABPlayerController::ShowResultUI()
         {
            ResultWidget->AddToViewport();
            ChangeInputMode(false);
         }
         ```

         </details>

   2. 게임 플레이가 종료되는 시점을 지정한다.
      - 플레이어가 죽었을 때와 목표를 달성했을때 종료되며 일단 테스트를 위해 두개의 섹션을 COMPLETE 스테이트로 만들면 목표를 달성한 것으로 한다.
         - 목표의 달성 여부는 게임의 정보임으로 GameState의 bGameCleared라는 속성을 추가하고 GameMode에서 목표가 달성되면 모든 Pawn을 멈추고 bGameCleared를 true로 설정한다.
         - 목표를 달성하거나 죽으면 플레이어 컨트롤러의 ShowResultUI 함수를 호출한다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABGameState.h
         public:
            void SetGameCleared();
            bool IsGameCleared() const;
         private:
            UPROPERTY(Transient)
            bool bGameCleared;
         //ABGameState.cpp
         AABPlayerState::AABPlayerState()
         {
            ...
            bGameCleared = false;
         }
         void AABPlayerState::SetGameCleared()
         {
            bGameCleared = true;
         }

         bool AABPlayerState::IsGameCleared() const
         {
            return bGameCleared;
         }

         //ABGameMode.h
         private:
            UPROPERTY()
            int32 ScoreToClear;
         //ABGameMode.cpp
         AABGameMode::AABGameMode()
         {
            ...
            ScoreToClear = 2;
         }
         void AABGameMode::AddScore(AABPlayerController* ScoredPlayer)
         {
            ...
            
            if(GetScore() >= ScoreToClear)	//목적의 달성 종료
            {
               ABGameState->SetGameCleared();
               
               for (FConstPawnIterator It = GetWorld()->GetPawnIterator();It;++It) (*It)->TurnOff();
               
               for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();It;++It)	//UI를 호출한
               {
                  const auto ABPlayerController = Cast<AABPlayerController>(It->Get());
                  if(nullptr != ABPlayerController) ABPlayerController->ShowResultUI();	
               }
            }
         }
         //ABCharactet.cpp
         void AABCharacter::SetCharacterState(ECharacterState NewState)
         {
            ...
            case ECharacterState::DEAD:
               ...
               GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void
               {
                  if(bIsPlayer) ABPlayerController->ShowResultUI();
                  else Destroy();
               }),DeadTimer, false);
               
               break;
            }
         }
         ```

         </details>

   3. UI 위젯이 게임스테이트의 정보를 읽어들일 수 있도록 바인딩을 설정
      - UI 위젯의 NativeConstruct 함수는 AddToViewport 함수가 외부에서 호출될 때 UI 위젯이 초기화 되며 호출된다.
      - ShowResultUI 함수에서 AddToViewport 함수전에 바인딩할 수 있도록하고 ABGameplaywidget 클래스에서 구현못한 btnRetryGame 버튼의 기능을 구현한다.

         <details><summary>코드 보기</summary>

         ```c++
         //ABGameplayResultWidget.h
         public:
            void BindGameState(class AABGameState* GameState);
         ...
         private:
            TWeakObjectPtr<class AABGameState> CurrentGameState;
         //ABGameplayResultWidget.cpp
         void UABGameplayResultWidget::BindGameState(AABGameState* GameState)
         {
            ABCHECK(nullptr != GameState);
            CurrentGameState = GameState;	//상태
         }

         void UABGameplayResultWidget::NativeConstruct()
         {
            Super::NativeConstruct();

            ABCHECK(CurrentGameState.IsValid());
            
            auto Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtResult")));
            ABCHECK(nullptr != Result);
            Result->SetText(FText::FromString(CurrentGameState->IsGameCleared() ? TEXT("Mission Complete") : TEXT("Mission Failed")));

            auto TotalScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("txtTotalScore")));
            ABCHECK(nullptr != TotalScore);
            TotalScore->SetText(FText::FromString(FString::FromInt(CurrentGameState->GetTotalGameScore())));
         }
         //ABGamePlayWidget.cpp
         void UABGameplayWidget::OnRetryGameClicked()
         {
            auto ABPlayerController = Cast<AABPlayerController>(GetOwningPlayer());
            ABCHECK(nullptr != ABPlayerController);
            ABPlayerController->RestartLevel();
         }
         //ABPlayerController.cpp
         #include "ABGameState.h"
         void AABPlayerController::ShowResultUI()
         {
            auto ABGameState = Cast<AABGameState>(UGameplayStatics::GetGameState(this));
            ABCHECK(nullptr != ABGameState);
            ResultWidget->BindGameState(ABGameState);
            ...
         }
         ```

         </details>

- ### 공격 판정을 로그로 저장하고 시각화하여 표현
   - <img src="Image/VisualLogger.gif" height="300" title="VisualLogger">
   - 비주얼 로거를 사용하면 진행 상황을 시각적으로 활용할 수 있어 문제점 파악에 효과적이다.
   - 창 > 개발자 툴 > 비주얼 로거에서 확인 가능하며 UE_VLOG로 시작하는 매크로를 사용해 생성가능하다.
   - 이전 디버그 드로잉과 유사하기만 비주얼 로거는 결과를 차후에 확인할 수 있다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABCharacter.cpp
      #include "VisualLogger/VisualLogger.h"
      ...
      void AABCharacter::AttackCheck()
      {
         ...
      #if ENABLE_DRAW_DEBUG
         ...
         DrawDebugCapsule(GetWorld(),Center,HalfHeight,AttackRadius,CapsuleRot,DrawColor,false,DebugLifeTime);

         //비주얼 로거
         UE_VLOG_LOCATION(this, Arena, Verbose, GetActorLocation(), 50.0f, FColor::Blue, TEXT("Attack Position"));
         UE_VLOG_CAPSULE(this, Arena, Verbose, GetActorLocation() - GetActorForwardVector() * AttackRadius, HalfHeight, AttackRadius, CapsuleRot, DrawColor, TEXT("Attak Arena"));
      }
      ```

      </details>

- ### 패키징 후 Fatal Error 발생
   1. LogPlayerController: Error: InputMode:UIOnly - Attempting to focus Non-Focusable widget SObjectWidget [Widget.cpp(799)]!

> **<h3>Realization</h3>**
- 게임 정지시 게임 중지, 마우스커서 출력, 입력이 UI에만 전달되도록 제어해야한다. 모두 PlayerController에서 제어한다.
   - PlayerController의 SetPause 함수를 사용하면 플레이를 일시 중지할 수 있다.
   - 마우스 커서도 bShowMouseCursor 속성을 true로 하면 보여지고 SetInputMode에 FinputModeUIOnly 클래스를 넣어 UI에만 입력되도록한다.
- 멀티플레이에서 캐릭터의 초기화는 PossessedBy 에서 구현이 올바르다.

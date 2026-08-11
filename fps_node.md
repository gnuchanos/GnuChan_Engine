                    FPSController
                    (KinematicBody)
                           │
          ┌────────────────┼────────────────┐
          │                │                │
          ▼                ▼                ▼
   MovementSystem      LookSystem      StateMachine
          │                │                │
     ┌────┴────┐       ┌───┴───┐      ┌────┼────┐
     │         │       │       │      │    │    │
  Gravity    Jump     Yaw    Pitch  Ground Air  Crouch
     │
     ▼
  Velocity
     │
     ▼
 move_and_slide()
     
FPSController
│
├── Head
│   └── Camera
│
├── CollisionShape
│
├── MovementSystem
│
├── LookSystem
│
├── StateMachine
│
├── InteractionSystem
│   └── RayCast
│
└── WeaponSystem
    └── WeaponPivot

inspectorde settings ...
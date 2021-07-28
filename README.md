# TruthEngine

I've already developed basic DirectX11 and DirectX12 versions of the engine separately. this unification will help in future engine development, particularly to support more platforms and APIs.
The Engine has a layered structure with abstractions which allows it to become independent from any OS or graphics API. for now it's work with Windows and DirectX12, using Nvidia Physx library for physics simulation and the "entt" library for Entity Component System. Also, it has an animation system that is capable of importing skinned animations.
The engine also supports multithreading and has got its own threadpool.

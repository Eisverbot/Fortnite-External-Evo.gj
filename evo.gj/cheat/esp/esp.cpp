#include "esp.h"

void Esp::ActorLoop() {
	//I will link dumps.host to each offset, on the website it might be outdated but it gives you a understanding of the sdk
	
	//get address
	{
		LocalPtrs::Gworld = read<uint64_t>(BaseId + 0x11E88988); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=offsets
		if (Debug::PrintPointers) Util::PrintPtr("Uworld: ", LocalPtrs::Gworld);
		if (!LocalPtrs::Gworld) return;

		uintptr_t OwningGameInstance = read<uint64_t>(LocalPtrs::Gworld + 0x1d8); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UWorld&member=OwningGameInstance
		if (Debug::PrintPointers) Util::PrintPtr("OwningGameInstance: ", OwningGameInstance);
		if (!OwningGameInstance) return;

		LocalPtrs::LocalPlayers = read<uint64_t>(read<uint64_t>(OwningGameInstance + 0x38)); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UGameInstance&member=LocalPlayers
		if (Debug::PrintPointers) Util::PrintPtr("LocalPlayers: ", LocalPtrs::LocalPlayers);
		if (!LocalPtrs::LocalPlayers) return;

		LocalPtrs::PlayerController = read<uint64_t>(LocalPtrs::LocalPlayers + 0x30); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UPlayer&member=PlayerController
		if (Debug::PrintPointers) Util::PrintPtr("PlayerController: ", LocalPtrs::PlayerController);
		if (!LocalPtrs::PlayerController) return;
		
		LocalPtrs::Player = read<uint64_t>(LocalPtrs::PlayerController + 0x330); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=APlayerController&member=Player
		if (Debug::PrintPointers) Util::PrintPtr("Player: ", LocalPtrs::Player);
		if (!LocalPtrs::Player) return;

		LocalPtrs::RootComponent = read<uint64_t>(LocalPtrs::Player + 0x198); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AActor&member=RootComponent
		if (Debug::PrintPointers) Util::PrintPtr("RootComponent: ", LocalPtrs::RootComponent);
		if (!LocalPtrs::RootComponent) return;

	}

	//get player array then loop through it
	{
		uintptr_t GameState = read<uintptr_t>(LocalPtrs::Gworld + 0x160); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UWorld&member=GameState
		if (Debug::PrintPointers) Util::PrintPtr("GameState: ", GameState);
		if (!GameState) return;

		uintptr_t PlayerArray = read<uintptr_t>(GameState + 0x2a8); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AGameStateBase&member=PlayerArray

		int Num = read<int>(GameState + (0x2a8 + sizeof(uintptr_t))); //reads the total number of player states in this array
		
		for (int i = 0; i < Num; i++) {
		

			uintptr_t PlayerState = read<uintptr_t>(PlayerArray + (i * sizeof(uintptr_t))); //the size of the pointer in the array we are reading is the size of uintptr_t
			if (Debug::PrintPointers) Util::PrintPtr("Enemy PlayerState: ", PlayerState);
			if (!PlayerState) continue; // we say if the pointer is invalid to continue, this means it will continue onto the next loop

			//since we only have the player state, we use PawnPrivate to get to the player
			uintptr_t Player = read<uintptr_t>(PlayerState + 0x308); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=APlayerState&member=PawnPrivate
			if (Debug::PrintPointers) Util::PrintPtr("Enemy Player: ", Player);
			if (!Player) continue;

			uintptr_t Mesh = read<uintptr_t>(Player + 0x318); //https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=ACharacter&member=Mesh
			if (Debug::PrintPointers) Util::PrintPtr("Enemy Mesh: ", Mesh);
			if (!Mesh) continue;


			Vector3 Head3D = SDK::GetBoneWithRotation(Mesh, 110);
			Vector2 Head2D = SDK::ProjectWorldToScreen(Head3D);
			if (Debug::PrintLocations) Util::Print3D("Head3D: ", Head3D);
			if (Debug::PrintLocations) Util::Print2D("Head2D: ", Head2D);

			Vector3 Bottom3D = SDK::GetBoneWithRotation(Mesh, 0);
			Vector2 Bottom2D = SDK::ProjectWorldToScreen(Bottom3D);

			float BoxHeight = (float)(Head2D.y - Bottom2D.y);
			float CornerHeight = abs(Head2D.y - Bottom2D.y);
			float CornerWidth = BoxHeight * 0.80;

			if (bCornerBox)
				Util::DrawCornerBox(Head2D.x - (CornerWidth / 2), Head2D.y, CornerWidth, CornerHeight, IM_COL32(0, 173, 237, 255), 1.5);
			
			auto dist = Util::GetCrossDistance(Head2D.x, Head2D.y, Width / 2, Height / 2);
			if (dist < FovSize && dist < ClosestDistance) {
				ClosestDistance = dist;
				TargetPawn = Player;
			}
		}
	}
}

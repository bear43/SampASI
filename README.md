# SampASI
For San Andreas Multiplayer 0.3.7 R3
Plugin allows you to listen to radio just in game. SA-MP has this function in samp.dll, plugin just calling the function from library.
Connect delay decreased from 3000 ms to 170 ms.
Added reconnect button.

Created RakNet hooks for send and receive RPC requests. Filtrating all requests by RPC_ID, where it equals RPC_DialogResponse and RPC_ShowDialog. Information about dialogs is displayed on imgui interface by pressing F3

Keyboards:
F3 - Menu
F12 - Detach plugin

Now attached ImGUI: https://github.com/ocornut/imgui.


AsyncWebSocket ws("/ws");

void initWebSockets()
{
    // Add event listener for new client connections
    ws.onEvent([&](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
               {
    switch (type) {
      case WS_EVT_CONNECT: // Client connected to the websocket
        Log.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT: // Client disconnected from the websocket
        Log.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:  // New data message received from the client
        if (len > 1) {
          // Send back to client the message received with an acknowledgment of received
          AwsFrameInfo * info = (AwsFrameInfo*)arg;
          String msg = "";
          if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            data[len] = 0; // Set the null terminator, just in case
            msg = (char*)data;
            Log.printf("WebSocket message received: %s\n", msg.c_str());
          }
        }
        break;
      case WS_EVT_PONG:  // Pong message received from the client
      case WS_EVT_ERROR: // An error occurred
        break;
    }; });

    server.addHandler(&ws);
}

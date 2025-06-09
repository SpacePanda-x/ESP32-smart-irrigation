import { WebSocketServer } from "ws";

const server = new WebSocketServer({ port: 8081 });
let valveSettings = {
  6: {
    name: "GPIO 6",
    active: true,
    use_ET0: false,
    mm: 0,
    frequency: 0,
    rate: 0,
    area: 0,
    Kc: 0,
  },
  4: {
    name: "GPIO 4",
    active: true,
    use_ET0: false,
    mm: 0,
    frequency: 0,
    rate: 0,
    area: 0,
    Kc: 0,
  },
  2: {
    name: "GPIO 2",
    active: true,
    use_ET0: false,
    mm: 0,
    frequency: 0,
    rate: 0,
    area: 0,
    Kc: 0,
  },
  8: {
    name: "GPIO 8",
    active: false,
    use_ET0: false,
    mm: 0,
    frequency: 0,
    rate: 0,
    area: 0,
    Kc: 0,
  },
};

server.on("connection", (socket) => {
  console.log("Client connected");

  // Handle incoming messages
  socket.on("message", (msg) => {
    console.log("Host Received:", JSON.parse(msg));
    msg = JSON.parse(msg);
    if (msg.type === "getValveSettings") {
      socket.send(
        JSON.stringify({ type: "valveSettings", payload: valveSettings })
      );
    }
    if (msg.type === "setValveSettings") {
      valveSettings = msg.payload;
    }
  });

  socket.on("close", () => {
    console.log("Client disconnected");
  });
});

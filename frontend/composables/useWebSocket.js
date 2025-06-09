import { ref } from "vue";

const socket = ref(null);
const messageHandlers = {};
const reconnectHandlers = [];
let isInitialized = false;
let readyResolve;
let ready = createReadyPromise();
let reconnectAttempts = 0;
const maxReconnectDelay = 10000; // 10 seconds

const isDev = process.env.NODE_ENV === "development";
// const wsUrl = isDev ? "ws://localhost:8081" : `ws://${location.host}`;

function createReadyPromise() {
  return new Promise((resolve) => {
    readyResolve = resolve;
  });
}

function connectWebSocket() {
  socket.value = new WebSocket("ws://localhost:8081");

  socket.value.onopen = () => {
    console.log("WebSocket connected");
    reconnectAttempts = 0;
    readyResolve();
    reconnectHandlers.forEach((cb) => cb());
  };

  socket.value.onmessage = (event) => {
    const { type, payload } = JSON.parse(event.data);
    if (messageHandlers[type]) {
      messageHandlers[type](payload);
    } else {
      console.warn("No handler for type:", type);
    }
  };

  socket.value.onclose = () => {
    console.warn("WebSocket closed. Attempting to reconnect...");
    scheduleReconnect();
  };

  socket.value.onerror = (err) => {
    console.error("WebSocket error", err);
    socket.value?.close(); // Triggers `onclose`
  };
}

function scheduleReconnect() {
  const delay = Math.min(1000 * 2 ** reconnectAttempts, maxReconnectDelay);
  setTimeout(() => {
    reconnectAttempts++;
    ready = createReadyPromise(); // Reset the ready promise
    connectWebSocket();
  }, delay);
}

export function useWebSocket() {
  if (process.client && !isInitialized) {
    connectWebSocket();
    isInitialized = true;
  }

  const send = async (type, payload = {}) => {
    await ready;
    if (socket.value?.readyState === WebSocket.OPEN) {
      socket.value.send(JSON.stringify({ type, payload }));
    } else {
      console.warn("WebSocket not open yet");
    }
  };

  const on = (type, callback) => {
    messageHandlers[type] = callback;
  };

  const onReconnect = (callback) => {
    reconnectHandlers.push(callback);
  };
  return { send, on, onReconnect };
}

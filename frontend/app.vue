<template>
  <AppNavbar />
  <NuxtPage />
</template>
<script setup>
import { useWebSocket } from "@/composables/useWebSocket";

useWebSocket(); // Initializes WebSocket on app load
</script>

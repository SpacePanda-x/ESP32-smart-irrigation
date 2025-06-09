<template>
  <main class="container mx-auto p-4 justify-center">
    <div v-if="!valveSettings || valveSettings.length === 0">
      <div class="flex justify-center items-center w-full pt-6">
        <span class="loading loading-bars loading-xl"></span>
      </div>
    </div>

    <div v-else>
      <div class="flex justify-center mb-6">
        <button class="btn btn-primary" @click="saveAll">💾 Save All</button>
      </div>

      <div
        class="grid gap-6 justify-items-center [grid-template-columns:repeat(auto-fit,minmax(250px,1fr))]"
      >
        <AppSettingsValveCard
          v-for="([key, valve], i) in valveSettings"
          :key="key"
          :gpio="key"
          :ref="(el) => (valveRefs[i] = el)"
          v-bind="valve"
        />
      </div>
    </div>
  </main>
</template>

<script setup>
import { ref, onMounted } from "vue";
import { useWebSocket } from "@/composables/useWebSocket";

const { send, on, onReconnect } = useWebSocket();
const valveSettings = ref(null);
const valveRefs = ref([]);

function loadData() {
  send("getValveSettings");
  on("valveSettings", (payload) => {
    valveSettings.value = Object.entries(payload);
    console.log("Got settings:", payload);
  });
}

onMounted(() => {
  loadData();
});

onReconnect(() => {
  loadData();
});

function saveAll() {
  const updatedEntries = valveSettings.value.map(([key, _valve], i) => {
    const updatedValve = valveRefs.value[i]?.getSettings?.();
    return [key, updatedValve];
  });

  const updatedPayload = Object.fromEntries(updatedEntries);
  send("setValveSettings", updatedPayload);
  console.log("updated setting: ", updatedPayload);
}
</script>

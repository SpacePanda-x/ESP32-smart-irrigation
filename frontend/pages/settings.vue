<template>
  <main class="container mx-auto p-4 justify-center">
    <div v-if="!valveSettings || valveSettings.length === 0">
      <div class="flex justify-center items-center w-full pt-6">
        <span class="loading loading-bars loading-xl"></span>
      </div>
    </div>

    <div v-else>
      <div class="flex justify-center my-4">
        <button class="btn btn-primary" @click="saveAll">💾 Save All</button>
      </div>
      <div class="toast toast-top toast-center">
        <div
          v-show="showAlert"
          class="alert alert-success shadow-lg transition-opacity duration-800"
          :class="{ 'opacity-0': !showAlert, 'opacity-100': showAlert }"
        >
          <svg
            xmlns="http://www.w3.org/2000/svg"
            class="h-6 w-6 shrink-0 stroke-current"
            fill="none"
            viewBox="0 0 24 24"
          >
            <path
              stroke-linecap="round"
              stroke-linejoin="round"
              stroke-width="2"
              d="M9 12l2 2 4-4m6 2a9 9 0 11-18 0 9 9 0 0118 0z"
            />
          </svg>
          <span>Settings saved successfully!</span>
        </div>
      </div>

      <div
        class="grid gap-6 justify-items-center [grid-template-columns:repeat(auto-fit,minmax(350px,1fr))]"
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
const showAlert = ref(false);

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

  showAlert.value = true;
  setTimeout(() => {
    showAlert.value = false;
  }, 5000);
}
</script>

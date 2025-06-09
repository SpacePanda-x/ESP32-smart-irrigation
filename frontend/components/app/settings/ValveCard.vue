<template>
  <fieldset
    class="fieldset bg-base-200 border-base-300 rounded-box border p-4 min-w-[250px] max-w-sm w-full shadow-sm"
  >
    <legend class="fieldset-legend">GPIO {{ props.gpio }}</legend>

    <label class="label">Name</label>
    <input
      v-model="localState.name"
      :disabled="!localState.active"
      type="text"
      class="input"
    />

    <label class="label">Password</label>
    <input
      :disabled="!localState.active"
      type="text"
      class="input"
      placeholder="Password"
    />

    <button
      class="btn mt-4"
      :class="localState.active ? 'btn-error' : 'btn-success'"
      @click="localState.active = !localState.active"
    >
      {{ localState.active ? "Deactivate" : "Activate" }}
    </button>
  </fieldset>
</template>

<script setup>
import { ref, watch } from "vue";

const props = defineProps({
  gpio: String,
  name: String,
  active: Boolean,
  use_ET0: Boolean,
  mm: Number,
  frequency: Number,
  rate: Number,
  area: Number,
  Kc: Number,
});

// Local state for editing
const localState = ref({
  name: props.name,
  active: props.active,
  use_ET0: props.use_ET0,
  mm: props.mm,
  frequency: props.frequency,
  rate: props.rate,
  area: props.area,
  Kc: props.Kc,
});

// Sync props into local state when they change
watch(props, (newProps) => {
  Object.assign(localState.value, newProps);
});

// Expose method to parent
defineExpose({
  getSettings() {
    return { ...localState.value };
  },
});
</script>

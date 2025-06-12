<template>
  <fieldset
    class="fieldset bg-base-200 border-base-300 rounded-box border p-4 min-w-[350px] max-w-sm w-full shadow-sm flex flex-col"
  >
    <legend class="fieldset-legend">GPIO {{ props.gpio }}</legend>

    <label class="label">Name</label>
    <input
      v-model="localState.name"
      :disabled="!localState.active"
      type="text"
      class="input w-full"
    />

    <!-- <div class="divider mt-4">Evapotranspiration calculation</div> -->

    <label class="label">Irrigation flow rate</label>
    <div class="join w-full">
      <div
        class="tooltip w-full"
        data-tip="Volume of water the irrigation will provide per unit time"
      >
        <input
          :disabled="!localState.active"
          v-model="inputBuffer.rate"
          @blur="commitRate"
          type="number"
          inputmode="decimal"
          class="input rounded-l-field join-item w-full"
        />
      </div>
      <div class="tooltip" data-tip="Click to change unit">
        <button
          :disabled="!localState.active"
          @click="cycleUnit"
          class="btn btn-neutral join-item w-20"
        >
          {{ selectedUnit }}
        </button>
      </div>
    </div>

    <label class="label">Irrigation area</label>
    <div class="join w-full">
      <div
        class="tooltip w-full"
        data-tip="Area covered by irrigation system. If irrigating a pot use the area of the pots surface (πr²)"
      >
        <input
          :disabled="!localState.active"
          v-model="inputBuffer.area"
          @blur="commitArea"
          type="number"
          inputmode="decimal"
          class="input rounded-l-field join-item w-full"
        />
      </div>
      <div class="tooltip" data-tip="Click to change unit">
        <button
          :disabled="!localState.active"
          @click="cycleAreaUnit"
          class="btn btn-neutral join-item w-20"
        >
          {{ selectedAreaUnit }}
        </button>
      </div>
    </div>

    <label class="label">Evapotranspiration crop coefficient (Kc)</label>
    <div class="text-2xl font-bold text-center" :disabled="!localState.active">
      {{ localState.Kc }}
    </div>
    <div
      class="tooltip tooltip-bottom"
      data-tip="Set the crop coefficient (Kc) to reflect your crop's canopy, height, and growth stage relative to 12cm of grass covering the same area. A dense, tall crop like corn might have Kc ≈ 1.1, while sparse crops like young vegetables might be closer to 0.4. Adjust Kc throughout the season as the crop develops."
    >
      <input
        type="range"
        min="0.1"
        max="2"
        step="0.1"
        v-model.number="localState.Kc"
        class="range"
        :disabled="!localState.active"
      />
    </div>
    <label class="label my-4">
      Crop is under cover?
      <div
        class="tooltip tooltip-left ml-auto"
        data-tip="By default, rainfall is counted against the water lost via evapotranspiration. If your crop is under cover and wont see rainfall, toggle this on."
      >
        <input
          v-model="localState.under_cover"
          :disabled="!localState.active"
          type="checkbox"
          class="toggle"
        />
      </div>
    </label>

    <!-- <label class="label mt-4">
      Schedule via evapotranspiration?
      <div
        class="tooltip tooltip-left ml-auto"
        data-tip="When toggled on, additional to being able to manually schedule irrigation events, irrigation will also be scheduled to replace the water lost via the calculated evapotranspiration of your crop."
      >
        <input
          v-model="localState.use_ET0"
          :disabled="!localState.active"
          type="checkbox"
          class="toggle toggle-primary"
        />
      </div>
    </label>

    <div v-if="localState.use_ET0" class="pt-3">
      <label class="label mt-4 w-full"> Irrigate based on: </label>
      <div class="flex justify-center pt-2">
        <div
          class="tooltip"
          data-tip="Irrigation will commence after the specified number of days has passed"
        >
          <input
            type="radio"
            :name="'irrigation-method-' + props.gpio"
            class="btn mx-2"
            :disabled="!localState.active"
            :checked="!localState.mm"
            @change="
              localState.frequency = 1;
              localState.mm = 0;
            "
            aria-label="days passed"
          />
        </div>
        <div
          class="tooltip"
          data-tip="Irrigation will commence after the specified mm of water has been lost"
        >
          <input
            type="radio"
            :name="'irrigation-method-' + props.gpio"
            class="btn mx-2"
            :disabled="!localState.active"
            :checked="localState.mm"
            @change="
              localState.frequency = 0;
              localState.mm = 1;
            "
            aria-label="water lost"
          />
        </div>
      </div>
      <div class="relative w-full flex justify-center my-4">
        <div class="relative flex items-center">
          <input
            v-if="localState.mm"
            v-model="inputBuffer.mm"
            @blur="commitMM"
            type="text"
            inputmode="decimal"
            class="input input-bordered text-3xl font-bold text-center w-24 h-16 leading-none"
            :disabled="!localState.active"
          />
          <input
            v-if="localState.frequency"
            v-model="inputBuffer.frequency"
            @blur="commitFrequency"
            type="text"
            inputmode="decimal"
            class="input input-bordered text-3xl font-bold text-center w-24 h-16 leading-none"
            :disabled="!localState.active"
          />
          <span
            v-if="localState.mm"
            class="absolute right-[-3rem] top-1/2 -translate-y-1/2 text-sm opacity-70"
          >
            mm
          </span>
          <span
            v-if="localState.frequency"
            class="absolute right-[-3rem] top-1/2 -translate-y-1/2 text-sm opacity-70"
          >
            days
          </span>
        </div>
      </div>

      <div class="w-full max-w-xs">
        <input
          v-if="localState.mm"
          type="range"
          min="1"
          max="50"
          step="1"
          v-model.number="localState.mm"
          class="range range-primary"
          :disabled="!localState.active"
        />

        <input
          v-if="localState.frequency"
          type="range"
          min="0.25"
          max="7"
          step="0.25"
          v-model.number="localState.frequency"
          class="range range-primary"
          :disabled="!localState.active"
        />
      </div>
    </div> -->
    <button
      class="btn mt-auto"
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
  under_cover: Boolean,
  mm: Number,
  frequency: Number,
  rate: Number,
  area: Number,
  Kc: Number,
});

// Local state for editing
const localState = reactive({
  name: props.name,
  active: props.active,
  use_ET0: props.use_ET0,
  under_cover: props.under_cover,
  mm: props.mm,
  frequency: props.frequency,
  rate: props.rate,
  area: props.area,
  Kc: props.Kc,
});

const inputBuffer = reactive({
  mm: String(localState.mm ?? ""),
  frequency: String(localState.frequency ?? ""),
  rate: String(localState.rate ?? ""),
  area: String(localState.area ?? ""),
});

const units = ["L/hr", "ml/min", "fl.oz/min", "gal/hr"];
const selectedUnitIndex = ref(0);
const selectedUnit = computed(() => units[selectedUnitIndex.value]);

const areaUnits = ["m²", "ft²", "yd²", "acre"];
const selectedAreaUnitIndex = ref(0);
const selectedAreaUnit = computed(() => areaUnits[selectedAreaUnitIndex.value]);

const conversionToLhr = {
  "L/hr": 1,
  "ml/min": 0.06, // 1 ml/min = 0.06 L/hr
  "fl.oz/min": 1.7734476, // 1 US fl oz/min = 1.7734476 L/hr
  "gal/hr": 3.785411784, // 1 US gal/hr = 3.785411784 L/hr
};
const conversionToM2 = {
  "m²": 1,
  "ft²": 0.09290304, // 1 ft² = 0.09290304 m²
  "yd²": 0.83612736, // 1 yd² = 0.83612736 m²
  acre: 4046.8564224, // 1 acre = 4046.8564224 m²
};

function cycleUnit() {
  const oldUnit = selectedUnit.value;

  // Step 1: Parse and convert current input value to L/hr
  const parsed = parseFloat(inputBuffer.rate);
  if (isNaN(parsed) || parsed <= 0) {
    // If invalid, keep index cycling but don’t update the input
    selectedUnitIndex.value = (selectedUnitIndex.value + 1) % units.length;
    return;
  }
  const asLhr = parsed * conversionToLhr[oldUnit];

  // Step 2: Switch to the next unit
  selectedUnitIndex.value = (selectedUnitIndex.value + 1) % units.length;
  const newUnit = selectedUnit.value;

  // Step 3: Convert from L/hr to new unit
  const newValue = asLhr / conversionToLhr[newUnit];
  inputBuffer.rate = +newValue.toLocaleString("fullwide", {
    useGrouping: false,
  });
}

function cycleAreaUnit() {
  const currentUnit = selectedAreaUnit.value;
  const currentInput = inputBuffer.area;

  if (!currentInput || currentInput <= 0) {
    selectedAreaUnitIndex.value =
      (selectedAreaUnitIndex.value + 1) % areaUnits.length;
    return;
  }

  // Convert to m² first
  const inM2 = currentInput * conversionToM2[currentUnit];

  // Switch unit
  selectedAreaUnitIndex.value =
    (selectedAreaUnitIndex.value + 1) % areaUnits.length;
  const newUnit = selectedAreaUnit.value;

  // Convert to new unit
  const newValue = inM2 / conversionToM2[newUnit];
  inputBuffer.area = +newValue.toLocaleString("fullwide", {
    useGrouping: false,
  });
}

// Sync props into local state when they change
watch(props, (newProps) => {
  Object.assign(localState, newProps);
});

watch(
  () => localState.mm,
  (val) => {
    inputBuffer.mm = val?.toString() ?? "";
  }
);

watch(
  () => localState.frequency,
  (val) => {
    inputBuffer.frequency = val?.toString() ?? "";
  }
);
watch(
  () => localState.rate,
  (val) => {
    const reverseMultiplier = 1 / conversionToLhr[selectedUnit.value];
    inputBuffer.rate = val ? (val * reverseMultiplier).toString() : "";
  }
);
watch(
  () => localState.area,
  (val) => {
    const reverseMultiplier = 1 / conversionToLhr[selectedUnit.value];
    inputBuffer.area = val ? (val * reverseMultiplier).toString() : "";
  }
);

function commitRate() {
  const parsed = parseFloat(inputBuffer.rate);
  if (!isNaN(parsed) && parsed > 0) {
    const multiplier = conversionToLhr[selectedUnit.value];
    localState.rate = parsed * multiplier;
  } else {
    const reverseMultiplier = 1 / conversionToLhr[selectedUnit.value];
    const original = localState.rate
      ? (localState.rate * reverseMultiplier).toString()
      : "";
    inputBuffer.rate = original;
  }
}
function commitArea() {
  const parsed = parseFloat(inputBuffer.area);
  if (!isNaN(parsed) && parsed > 0) {
    const multiplier = conversionToM2[selectedAreaUnit.value];
    localState.area = parsed * multiplier;
  } else {
    const reverseMultiplier = 1 / conversionToM2[selectedAreaUnit.value];
    const original = localState.area
      ? (localState.area * reverseMultiplier).toString()
      : "";
    inputBuffer.area = original;
  }
}

function commitMM() {
  const val = parseFloat(inputBuffer.mm);
  if (!isNaN(val) && val > 0) {
    localState.mm = val;
  } else {
    inputBuffer.mm = localState.mm?.toString() ?? "";
  }
}

function commitFrequency() {
  const val = parseFloat(inputBuffer.frequency);
  if (!isNaN(val) && val > 0) {
    localState.frequency = val;
  } else {
    inputBuffer.frequency = localState.frequency?.toString() ?? "";
  }
}

// Expose method to parent
defineExpose({
  getSettings() {
    return toRaw(localState);
  },
});
</script>

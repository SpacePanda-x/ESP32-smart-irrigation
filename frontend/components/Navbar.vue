<template>
  <div class="navbar bg-base-100 px-4 shadow">
    <!-- Burger Menu -->
    <button @click="$emit('toggleSidebar')" class="btn btn-square btn-ghost">
      <svg
        xmlns="http://www.w3.org/2000/svg"
        fill="none"
        viewBox="0 0 24 24"
        stroke-width="1.5"
        stroke="currentColor"
        class="size-6"
      >
        <path
          stroke-linecap="round"
          stroke-linejoin="round"
          d="M3.75 6.75h16.5M3.75 12h16.5m-16.5 5.25h16.5"
        />
      </svg>
    </button>
    <NuxtLink to="/" class="flex items-center justify-center gap-2">
      <img class="w-8 h-8" src="/logo.svg" alt="logo" />
      <span class="text-2xl font-bold">Smart Irrigation</span>
    </NuxtLink>
  </div>
</template>

<script setup>
defineEmits(["toggleSidebar"]);
</script>

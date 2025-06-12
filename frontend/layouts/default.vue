<template>
  <div class="drawer" :class="{ 'lg:drawer-open': isSidebarOpen }">
    <input id="left-sidebar-drawer" type="checkbox" class="drawer-toggle" />

    <!-- Main Content -->
    <div class="drawer-content flex flex-col">
      <Navbar @toggle-sidebar="toggleSidebar" />
      <main class="flex-1 p-6">
        <slot />
      </main>
    </div>

    <!-- Sidebar -->
    <div class="drawer-side">
      <label
        for="left-sidebar-drawer"
        aria-label="close sidebar"
        class="drawer-overlay"
      ></label>
      <Sidebar />
    </div>
  </div>
</template>

<script setup>
const isSidebarOpen = ref(true);

// This function handles the toggle for both mobile and desktop
const toggleSidebar = () => {
  // For mobile, the checkbox is the source of truth
  const drawerCheckbox = document.getElementById("left-sidebar-drawer");
  if (drawerCheckbox) {
    // If we are on a smaller screen, the drawer is controlled by the checkbox
    if (window.innerWidth < 1024) {
      drawerCheckbox.checked = !drawerCheckbox.checked;
    } else {
      // On larger screens, we toggle our ref
      isSidebarOpen.value = !isSidebarOpen.value;
    }
  }
};

// Ensure sidebar is open by default on mount for desktop
onMounted(() => {
  if (window.innerWidth >= 1024) {
    isSidebarOpen.value = true;
  } else {
    isSidebarOpen.value = false;
  }
});
</script>

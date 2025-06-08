import tailwindcss from "@tailwindcss/vite";
export default defineNuxtConfig({
  compatibilityDate: '2025-05-15',
  devtools: { enabled: true },
  vite: {
    plugins: [tailwindcss()],
  },
  css: ["~/assets/app.css"],
  ssr: true,
  nitro: {
    preset: 'static'
  },
  app: {
    baseURL: '/',
  }
})

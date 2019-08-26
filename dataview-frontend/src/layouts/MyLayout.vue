<template>
  <div class="q-pa-md">
    <q-layout
      view="lhHh lpR fFf"
      container
      class="shadow-2 rounded-borders"
      style="height:500px"
    >
      <q-header reveal bordered class="bg-white text-primary">
        <q-toolbar>
          <q-toolbar-title>Garden View</q-toolbar-title>

          <div>Quasar v{{ $q.version }}</div>
        </q-toolbar>
      </q-header>

      <q-page-container>
        <router-view />
      </q-page-container>

      <q-footer bordered class="bg-white text-primary">
        <q-tabs
          no-caps
          active-color="primary"
          indicator-color="transparent"
          class="text-grey"
          v-model="tab"
        >
          <q-route-tab to="/" label="Overview" icon="ion-ios-partly-sunny" />
          <q-route-tab to="/graphs" label="Graphs" icon="ion-ios-trending-up" />
          <q-route-tab to="/raw" label="Data" icon="ion-ios-list" />
        </q-tabs>
      </q-footer>
    </q-layout>
  </div>
</template>

<script>
import { openURL } from "quasar";

export default {
  name: "MyLayout",
  data() {
    return {
      leftDrawerOpen: this.$q.platform.is.desktop
    };
  },
  methods: {
    openURL
  }
};
</script>

<style></style>

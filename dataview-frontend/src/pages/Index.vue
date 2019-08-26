<template>
  <q-page class="flex flex-center">
    <weather :info="latestSensorData"></weather>
    <weather :info="yesterdaySensorData"></weather>
    <weather :info="lastYearSensorData" v-if="hasLastYear"></weather>
  </q-page>
</template>

<style></style>

<script>
import Weather from "../components/Weather.vue";
import { mapState, mapGetters } from "vuex";

export default {
  name: "PageIndex",
  components: {
    Weather
   },
  
  computed: {
    ...mapState("sensor", ["sensorData"]),
    ...mapGetters("sensor", [
      "latestSensorData",
      "yesterdaySensorData",
      "lastYearSensorData"
    ]),
    hasLastYesterday: function() {
      return this.$store.getters["sensor/yesterdaySensorData"] !== undefined;
    },
    hasLastYear: function() {
      return this.$store.getters["sensor/lastYearSensorData"] !== undefined;
    }
  }
};
</script>

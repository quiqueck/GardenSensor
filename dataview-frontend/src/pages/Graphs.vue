<template>
    <q-page class="flex flex-center">
        <graph
            :data="[temperatures, humidity, voltage, moisture, brightness]"
        ></graph>
    </q-page>
</template>

<style></style>

<script>
import Graph from "../components/Graph.vue";
import moment from "moment";
import { mapState } from "vuex";

export default {
    name: "GraphData",
    data: function() {
        return {
            interval: null,
            range: 3,
            rangeUnit: "day"
        };
    },
    components: {
        Graph
    },
    computed: {
        ...mapState("sensor", ["sensorData"]),
        temperatures() {
            return this.createSet("Temperatur", "line-red", "temperature");
        },
        humidity() {
            return this.createSet("Luftfeuchtigkeit", "line-blue", "humidity");
        },
        voltage() {
            return this.createMappedSet("Spannung", "line-lightblue", i =>
                this.$store.getters["sensor/pVoltage"](i["voltage"])
            );
        },
        moisture() {
            return this.createMappedSet("Bodenfeuchtigkeit", "line-green", i =>
                this.$store.getters["sensor/pMoisture"](i["moisture"])
            );
        },
        brightness() {
            return this.createSet("Helligkeit", "line-orange", "brightness");
        }
    },
    methods: {
        createSet(label, clas, field) {
            return this.createMappedSet(label, clas, i => i[field]);
        },
        createMappedSet(label, clas, mapping) {
            const refTime = moment().subtract(this.range, this.rangeUnit);
            return {
                label: label,
                class: clas,
                values: this.sensorData.data
                    .filter(i => {
                        return moment(i.date).isAfter(refTime);
                    })
                    .map(i => {
                        return {
                            value: mapping(i),
                            date: new Date(i.date)
                        };
                    })
            };
        }
    },
    created() {
        console.log("Creating", this.doForcedUpdates);
        this.interval = setInterval(() => {
            this.$forceUpdate();
        }, 10000);
    },
    beforeDestroy() {
        console.log("Clearing");
        clearInterval(this.interval);
    }
};
</script>

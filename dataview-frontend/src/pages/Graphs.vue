<template>
    <q-page class="flex">
        <graph :values="displayedData"></graph>
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
        displayedData() {
            return {
                id: moment().format(moment.HTML5_FMT.DATETIME_LOCAL_SECONDS),
                data: [
                    this.temperatures,
                    this.humidity,
                    this.voltage,
                    this.moisture,
                    this.brightness
                ]
            };
        },
        values() {
            return this.sensorData.data;
        },
        temperatures() {
            return this.createSet(
                "Temperatur",
                "line-red",
                "°C",
                "temperature"
            );
        },
        humidity() {
            return this.createSet(
                "Luftfeuchtigkeit",
                "line-blue",
                "%",
                "humidity"
            );
        },
        voltage() {
            return this.createMappedSet("Spannung", "line-lightblue", "%", i =>
                this.$store.getters["sensor/pVoltage"](i["voltage"])
            );
        },
        moisture() {
            return this.createFilteredMappedSet(
                "Bodenfeuchtigkeit",
                "line-green",
                "%",
                (i, refTime) =>
                    i.moisture_cached === false &&
                    moment(i.date).isAfter(refTime),
                i => this.$store.getters["sensor/pMoisture"](i["moisture"])
            );
        },
        brightness() {
            return this.createSet(
                "Helligkeit",
                "line-orange",
                "lx",
                "brightness"
            );
        }
    },
    mounted() {
        window.addEventListener("resize", this.onResize);
        this.onResize();
    },
    methods: {
        onResize() {
            if (this.$el.offsetWidth < 4 * (24 * 6)) {
                this.range = 1;
            } else if (this.$el.offsetWidth < 4 * (2 * 24 * 6)) {
                this.range = 2;
            } else {
                this.range = 3;
            }
        },
        createSet(label, clas, unit, field) {
            return this.createMappedSet(label, clas, unit, i => i[field]);
        },
        createMappedSet(label, clas, unit, mapping) {
            return this.createFilteredMappedSet(
                label,
                clas,
                unit,
                (i, refTime) => moment(i.date).isAfter(refTime),
                mapping
            );
        },
        createFilteredMappedSet(label, clas, unit, filter, mapping) {
            const refTime = moment().subtract(this.range, this.rangeUnit);
            return {
                label: label,
                class: clas,
                unit: unit,
                values: this.values
                    .filter(i => {
                        return filter(i, refTime);
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
        this.interval = setInterval(() => {
            this.$forceUpdate();
        }, 10000);
    },
    beforeDestroy() {
        clearInterval(this.interval);
        window.removeEventListener("resize", this.onResize);
    }
};
</script>

<template>
    <q-card class="my-card">
        <q-card-section>
            <graph :values="displayedData"></graph>
        </q-card-section>
        <q-separator></q-separator>
        <q-card-section>
            <q-toggle
                v-model="showTemperature"
                color="red"
                icon="ion-ios-thermometer"
                label="Temperatur"
            ></q-toggle>

            <q-toggle
                v-model="showHumidity"
                color="indigo-5"
                icon="ion-ios-water"
                label="Luftfeuchtigkeit"
            ></q-toggle>

            <q-toggle
                v-model="showBrightness"
                color="orange"
                icon="ion-ios-sunny"
                label="Helligkeit"
            ></q-toggle>

            <q-toggle
                v-model="showBrightnessMT"
                color="pink-14"
                icon="ion-ios-contract"
                label="Sensitivität Helligkeit"
            ></q-toggle>

            <q-toggle
                v-model="showMoisture"
                color="green"
                icon="ion-ios-leaf"
                label="Bodenfeuchtigkeit"
            ></q-toggle>

            <q-toggle
                v-model="showVoltage"
                color="blue-11"
                icon="ion-ios-battery-charging"
                label="Spannung"
            ></q-toggle>
        </q-card-section>
    </q-card>
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
            rangeUnit: "day",
            showTemperature: true,
            showHumidity: true,
            showBrightness: true,
            showMoisture: true,
            showVoltage: true,
            showBrightnessMT: false
        };
    },
    components: {
        Graph
    },
    computed: {
        ...mapState("sensor", ["sensorData"]),
        displayedData() {
            let datasets = [];
            if (this.showTemperature) datasets.push(this.temperatures);
            if (this.showHumidity) datasets.push(this.humidity);
            if (this.showVoltage) datasets.push(this.voltage);
            if (this.showMoisture) datasets.push(this.moisture);
            if (this.showBrightness) datasets.push(this.brightness);
            if (this.showBrightnessMT) datasets.push(this.brightnessMT);
            return {
                id: moment().format(moment.HTML5_FMT.DATETIME_LOCAL_SECONDS),
                data: datasets
            };
        },
        values() {
            return this.sensorData.data;
        },
        temperatures() {
            return this.createSet("Temperatur", "red", "°C", "temperature");
        },
        humidity() {
            return this.createSet(
                "Luftfeuchtigkeit",
                "indigo-5",
                "%",
                "humidity"
            );
        },
        voltage() {
            return this.createMappedSet("Spannung", "blue-11", "%", i =>
                this.$store.getters["sensor/pVoltage"](i["voltage"])
            );
        },
        moisture() {
            return this.createFilteredMappedSet(
                "Bodenfeuchtigkeit",
                "green",
                "%",
                (i, refTime) =>
                    i.moisture_cached === false &&
                    moment(i.date).isAfter(refTime),
                i => this.$store.getters["sensor/pMoisture"](i["moisture"])
            );
        },
        brightness() {
            return this.createSet("Helligkeit", "orange", "lx", "brightness");
        },
        brightnessMT() {
            return this.createSet(
                "Sensitivität Helligkeit",
                "pink-14",
                "",
                "brightness_mt"
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
            const regex = /[^-\s]+-(\S+)\b/;
            const match = regex.exec(clas);
            return {
                label: label,
                class: clas,
                color: match && match.length > 0 ? match[1] : "gray",
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

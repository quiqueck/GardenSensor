<template>
    <q-card flat bordered class="my-card">
        <q-card-section class="bg-primary text-white">
            <div class="text-h6">{{ info.date | MomentDate }}</div>
            <div class="text-subtitle1 light">{{ momentDateFull }}</div>
        </q-card-section>
        <q-separator flat></q-separator>
        <q-card-section>
            <div class="row">
                <div class="text-caption value col-sm-4 col-xs-6">
                    <q-icon name="ion-ios-thermometer" class="large"></q-icon>
                    <br />
                    {{ temperature }}&thinsp;°C
                </div>
                <div class="text-caption value col-sm-4 col-xs-6">
                    <q-icon name="ion-ios-water" class="large"></q-icon>
                    <br />
                    {{ info.humidity }}&thinsp;%
                </div>
                <div class="text-caption value col-sm-4 col-xs-12">
                    <q-icon name="ion-ios-sunny" class="large"></q-icon>
                    <br />
                    {{ brightness }}&thinsp;{{ brightnessUnit }}
                </div>

                <div class="text-caption value col-sm-6 col-xs-6 padTop">
                    <q-circular-progress
                        show-value
                        :class="moistClass"
                        v-model="moisture"
                        size="75px"
                        :color="moistColor"
                        track-color="grey-3"
                    >
                        <div>
                            <q-icon name="ion-ios-leaf"></q-icon>
                            <br />
                            {{ moisture }}&thinsp;%
                        </div>
                    </q-circular-progress>
                </div>

                <div class="text-caption value col-sm-6 col-xs-6 padTop">
                    <q-circular-progress
                        show-value
                        class="text-red q-ma-md"
                        v-model="voltage"
                        size="75px"
                        color="red"
                        track-color="grey-3"
                    >
                        <div>
                            <q-icon name="ion-ios-battery-charging"></q-icon>
                            <br />
                            {{ voltage }}&thinsp;%
                        </div>
                    </q-circular-progress>
                </div>
            </div>
        </q-card-section>
    </q-card>
</template>

<script>
import { QCard } from "quasar";
import { MomentDate, MomentDateFull } from "../filters.js";
export default {
    filters: {
        MomentDate
    },
    props: {
        info: {
            type: Object,
            required: true
        }
    },
    data: function() {
        return {
            interval: null
        };
    },
    computed: {
        moistClass: function() {
            if (this.info === undefined) return "text-gray q-ma-md";
            return this.info.moisture_cached
                ? "text-gray q-ma-md"
                : "text-green q-ma-md";
        },
        moistColor: function() {
            if (this.info === undefined) return "text-gray q-ma-md";
            return this.info.moisture_cached ? "gray" : "green";
        },
        moisture: function() {
            if (this.info === undefined) return "0";
            return this.$store.getters["sensor/pMoisture"](this.info.moisture);
        },
        voltage: function() {
            if (this.info === undefined) return "0";
            return this.$store.getters["sensor/pVoltage"](this.info.voltage);
        },
        brightnessUnit: function() {
            if (this.info === undefined) return "lx";
            if (this.info.brightness > 1000) return "Klx";
            return "lx";
        },
        brightness: function() {
            if (this.info === undefined) return 0;
            if (this.info.brightness > 1000) {
                return Math.round(this.info.brightness / 100) / 10;
            }
            return Math.round(this.info.brightness);
        },
        temperature: function() {
            if (this.info === undefined) return 0.0;
            return Math.round(this.info.temperature * 10) / 10;
        },
        momentDate: function() {
            if (this.info === undefined) return MomentDate(undefined);

            return MomentDate(this.info.date);
        },
        momentDateFull: function() {
            if (this.info === undefined) return MomentDateFull(undefined);

            return MomentDateFull(this.info.date);
        }
    },
    methods: {},
    components: {
        QCard
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

<style scoped>
.my-card {
    margin: 8px;
}
.light {
    color: rgb(198, 198, 198);
    margin-top: -10px;
}

.value {
    font-size: 120%;
    font-weight: bold;
    text-align: center;
    align-content: center;
}

.large {
    font-size: 2em !important;
}

.padTop {
    padding-top: 20px;
}
</style>

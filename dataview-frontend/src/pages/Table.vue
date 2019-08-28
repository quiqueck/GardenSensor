<template>
    <q-page>
        <q-table
            title="Rohdaten"
            dense
            :data="sensorData.data"
            :columns="columns"
            row-key="name"
            :pagination.sync="pagination"
        >
            <q-td
                :style="moistureStyle(props)"
                slot="body-cell-moisture"
                slot-scope="props"
                :props="props"
            >
                {{ props.value }}
            </q-td>
        </q-table>
    </q-page>
</template>

<style></style>

<script>
import { mapState } from "vuex";
import { MomentDateFull } from "../filters.js";

export default {
    name: "RawData",
    data: function() {
        return {
            interval: null,
            pagination: {
                rowsPerPage: 7
            },
            columns: [
                {
                    name: "time",
                    label: "Zeit",
                    field: "date",
                    sortable: true,
                    format: val => `${MomentDateFull(val)}`
                },
                {
                    name: "temperature",
                    label: "Temperatur",
                    field: "temperature",
                    sortable: true,
                    format: val => `${val}°C`
                },
                {
                    name: "humidity",
                    label: "Luft",
                    field: "humidity",
                    sortable: true,
                    format: val => `${val}%`
                },
                {
                    name: "brightness",
                    label: "Licht",
                    field: "brightness",
                    sortable: true,
                    format: val => `${val} lx`
                },
                {
                    name: "moisture",
                    label: "Boden",
                    field: "moisture",
                    sortable: true,
                    format: val =>
                        `${this.$store.getters["sensor/pMoisture"](
                            val
                        )}% (${val})`
                },
                {
                    name: "voltage",
                    label: "Spannung",
                    field: "voltage",
                    sortable: true,
                    format: val =>
                        `${this.$store.getters["sensor/pVoltage"](
                            val
                        )}% (${val})`
                }
            ]
        };
    },
    methods: {
        moistureStyle: function(c) {
            if (c.row.moisture_cached) return "color:silver";
            return "";
        }
    },
    components: {},
    computed: {
        ...mapState("sensor", ["sensorData"])
    },
    created() {
        console.log("Creating", this.doForcedUpdates);
        this.interval = setInterval(() => {
            console.log("???");
            this.$forceUpdate();
        }, 10000);
    },
    beforeDestroy() {
        console.log("Clearing");
        clearInterval(this.interval);
    }
};
</script>

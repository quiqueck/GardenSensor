<template>
    <div id="q-app">
        <router-view />
    </div>
</template>

<script>
export default {
    name: "App",
    data: function() {
        return { interval: null };
    },
    mounted() {
        this.$store.dispatch("sensor/loadInitialSensorData").then(() => {
            this.interval = setInterval(() => {
                this.periodicUpdate();
            }, 10000);
        });
    },
    methods: {
        periodicUpdate: function() {
            //console.log("Starting Update");
            this.$store.dispatch("sensor/loadNewSensorData").then(() => {
                //console.log("Dispatching next Update");
                //setTimeout(this.periodicUpdate.bind(this), 10000);
            });
        }
    },
    beforeDestroy() {
        console.log("Clearing");
        clearInterval(this.interval);
    }
};
</script>

<style></style>

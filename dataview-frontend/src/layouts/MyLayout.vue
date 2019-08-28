<template>
    <div class="q-pa-md">
        <q-layout view="hHh lpR fFf">
            <q-header reveal bordered class="bg-white text-primary">
                <q-toolbar>
                    <q-tabs
                        no-caps
                        dense
                        align="left"
                        :breakpoint="0"
                        active-color="primary"
                        indicator-color="transparent"
                        class="text-grey"
                        v-model="tab"
                    >
                        <q-route-tab
                            to="/"
                            label="Overview"
                            icon="ion-ios-partly-sunny"
                        />
                        <q-route-tab
                            to="/graphs"
                            label="Graphs"
                            icon="ion-ios-trending-up"
                        />
                        <q-route-tab
                            to="/raw"
                            label="Data"
                            icon="ion-ios-list"
                        />
                    </q-tabs>

                    <q-space />
                    <q-separator dark vertical inset></q-separator>

                    <q-toolbar-title shrink>
                        Garden View
                        <span class="text-caption">
                            ({{ versionString }})
                        </span>
                    </q-toolbar-title>
                </q-toolbar>
            </q-header>

            <q-page-container>
                <router-view />
            </q-page-container>
        </q-layout>
    </div>
</template>

<script>
import { openURL } from "quasar";

export default {
    name: "MyLayout",
    data() {
        return {
            tab: null,
            versionString: process.env.APP_VERSION
        };
    },
    methods: {
        openURL
    }
};
</script>

<style></style>

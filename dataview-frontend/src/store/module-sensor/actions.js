import axios from "axios";
import moment from "moment";

const basePath = process.env.DEV ? "http://localhost:7056/api/001" : "/api/001";
console.log(process.env.APP_VERSION);
export const loadNewSensorData = ({ commit, state }) => {
    //console.log(commit, state.latestID);
    if (state.latestID > 0) {
        axios
            .get(basePath + "/data/json/afterID/" + state.latestID)
            .then(r => r.data)
            .then(sensorData => {
                //console.log(sensorData);
                if (sensorData.data && sensorData.data.length > 0) {
                    commit("addSensorReadings", sensorData);
                }
            });
    }
};

export const loadInitialSensorData = ({ commit }) => {
    axios
        .get(basePath + "/data/json/latest/600")
        .then(r => r.data)
        .then(sensorData => {
            commit("setSensorReadings", sensorData);

            const start = moment()
                .subtract(1, "year")
                .subtract(1, "day")
                .format();
            const end = moment()
                .subtract(1, "year")
                .format();

            axios
                .get(basePath + "/data/json/after/" + start + "/before/" + end)
                .then(r => r.data)
                .then(sensorDataLastYear => {
                    commit("addOlderSensorReadings", sensorDataLastYear);
                });
        });
};

import moment from "moment";

export const latestSensorData = state => {
    if (state.sensorData.data && state.sensorData.data.length > 0)
        return state.sensorData.data[0];

    return state.empty;
};

function timedData(amount, type, state) {
    if (state.sensorData.data && state.sensorData.data.length > 0) {
        return state.sensorData.data.find(i => {
            return moment()
                .subtract(amount, type)
                .isAfter(moment(i.date));
        });
    }

    return state.empty;
}

export const yesterdaySensorData = state => {
    return timedData(1, "day", state);
};

export const lastYearSensorData = state => {
    return timedData(1, "year", state);
};

export const pVoltage = state => v => {
    return Math.round(
        (100 * (v - state.minVoltage)) / (state.maxVoltage - state.minVoltage)
    );
};

export const pMoisture = state => v => {
    return (
        100 -
        Math.round(
            (100 * (v - state.minMoisture)) /
                (state.maxMoisture - state.minMoisture)
        )
    );
};

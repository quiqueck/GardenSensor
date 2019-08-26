import Vue from "Vue";

export const setSensorReadings = (state, sensorData) => {
  state.sensorData = sensorData;
  Vue.set(state.sensorData, "data", sensorData.data.reverse());
  state.latestID = state.sensorData.data[0].id;
};

export const addOlderSensorReadings = (state, olderSensorData) => {
  const res = state.sensorData.data.concat(olderSensorData.data.reverse());
  Vue.set(state.sensorData, "data", res);
  state.latestID = res[0].id;
};

export const addSensorReadings = (state, newSensorData) => {
  const res = newSensorData.data.reverse().concat(state.sensorData.data);
  Vue.set(state.sensorData, "data", res);
  state.latestID = res[0].id;
};

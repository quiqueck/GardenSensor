const defaultValues = {
    minVoltage: 13,
    maxVoltage: 260,
    minMoisture: 10,
    maxMoisture: 460
};

export default () => {
    return {
        ...defaultValues,
        empty: {
            date: undefined,
            temperature: 0,
            humidity: 0,
            voltage: defaultValues.minVoltage,
            brightness: 0,
            moisture: defaultValues.maxMoisture,
            moisture_cached: true,
            brightness_mt: 0,
            id: 0
        },
        sensorData: {
            data: [],
            date: new Date()
        },
        latestID: 0
    };
};

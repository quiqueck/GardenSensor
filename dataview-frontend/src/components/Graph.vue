<template>
    <div style="width:100%;position:relative">
        <svg
            ref="svg"
            @mousemove="mouseover"
            :width="areaWidth"
            :height="height"
        >
            <g
                :style="{
                    transform: `translate(${margin.left}px, ${margin.top}px)`
                }"
            >
                <path
                    v-for="path in paths"
                    :key="path.id"
                    :class="path.class"
                    :d="path.line"
                />
                <path class="selector" :d="selector" />
            </g>
            <g ref="xAxis"></g>
        </svg>

        <div ref="stats" class="info"></div>
    </div>
</template>

<script>
import * as d3 from "d3";

export default {
    name: "LineGraph",
    data: function() {
        return {
            areaWidth: 200,
            paths: [{ line: [], points: [], id: 0 }],
            scaled: { x: null, y: null },
            selector: null,
            lastHoverPoint: {}
        };
    },
    props: {
        data: {
            type: Array,
            default: () => [{ label: "", class: "line", values: [] }]
        },
        margin: {
            type: Object,
            default: function() {
                return { top: 20, right: 40, bottom: 60, left: 50 };
            }
        },
        height: {
            type: Number,
            default: 400
        }
    },
    computed: {
        hasSingle() {
            return this.data.length == 1;
        },
        padded() {
            const width = this.areaWidth - this.margin.left - this.margin.right;
            const height = this.height - this.margin.top - this.margin.bottom;
            return { width, height };
        },
        interpolationType() {
            //d3.curveLinear, d3.curveStep, d3.curveStepBefore, d3.curveStepAfter, d3.curveBasis, d3.curveCardinal, d3.curveMonotoneX, d3.curveCatmullRom

            return d3.curveCatmullRom;
        }
    },
    mounted() {
        window.addEventListener("resize", this.onResize);
        this.onResize();
        this.update();
    },
    beforeDestroy() {
        window.removeEventListener("resize", this.onResize);
    },
    watch: {
        areaWidth: function widthChanged() {
            console.log("Changed Size", this.areaWidth, this.height);
            this.update();
        }
    },
    methods: {
        onResize() {
            console.log(
                "OnResize",
                this.areaWidth,
                this.$el.offsetWidth,
                this.$el
            );
            this.areaWidth = this.$el.offsetWidth;
            //this.height = this.$el.offsetHeight;
        },
        createLine: function(points) {
            return d3
                .line()
                .curve(this.interpolationType)
                .x(d => d.x)
                .y(d => d.y)(points);
        },
        createValueSelector: d3
            .area()
            .x(d => d.x)
            .y0(d => d.max)
            .y1(0),
        update() {
            let updatedPaths = [];
            let mainDateScale = null;
            this.data.forEach(dataElement => {
                let updatedPath = {
                    id: dataElement.label,
                    class: "line " + dataElement.class,
                    line: [],
                    points: [],
                    scaled: {
                        x: d3.scaleTime().range([0, this.padded.width]),
                        y: d3.scaleLinear().range([this.padded.height, 0])
                    }
                };

                d3.axisLeft().scale(updatedPath.scaled.x);
                if (mainDateScale === null) {
                    d3.axisBottom().scale(updatedPath.scaled.y);
                    mainDateScale = d3.extent(dataElement.values, d => d.date);
                }
                console.log("Update", this.data);
                updatedPath.scaled.x.domain(mainDateScale);
                updatedPath.scaled.y.domain([
                    d3.min(dataElement.values, d => d.value),
                    d3.max(dataElement.values, d => d.value)
                ]);
                let points = [];
                dataElement.values.forEach(d => {
                    points.push({
                        x: updatedPath.scaled.x(d.date),
                        y: updatedPath.scaled.y(d.value),
                        max: this.height,
                        val: d
                    });
                });
                updatedPath.line = this.createLine(points);
                updatedPath.points = points;

                updatedPaths.push(updatedPath);
            });
            this.paths = updatedPaths;

            if (updatedPaths.length > 0) {
                const mainPath = updatedPaths[0];
                console.log("In", this.$refs.svg);
                let svg = d3.select(this.$refs.svg);

                svg.selectAll("#Axis").remove();

                // Add the X Axis
                svg.append("g")
                    .attr("id", "Axis")
                    .attr(
                        "style",
                        "transform:translate(" +
                            this.margin.left +
                            "px," +
                            (this.padded.height + 20) +
                            "px)"
                    )
                    .call(d3.axisBottom(mainPath.scaled.x));

                // Add the Y0 Axis
                svg.append("g")
                    .attr("id", "Axis")
                    .attr(
                        "style",
                        "transform:translate(" +
                            this.margin.left +
                            "px, " +
                            this.margin.top +
                            "px)"
                    )
                    .attr("class", "axisSteelBlue")
                    .call(d3.axisLeft(mainPath.scaled.y));
            }
        },
        mouseover(m) {
            const { offsetX } = m;
            //console.log(m);

            this.paths.forEach((path, idx) => {
                if (path.points.length > 0) {
                    const x = offsetX - this.margin.left;
                    const closestPoint = this.getClosestPoint(x);
                    if (this.lastHoverPoint.index !== closestPoint.index) {
                        const point = path.points[closestPoint.index];

                        if (idx == 0) {
                            this.selector = this.createValueSelector([point]);
                            this.$emit(
                                "select",
                                this.data[0].values[closestPoint.index]
                            );
                            this.lastHoverPoint = closestPoint;
                        }
                    }
                }
            });
        },
        getClosestPoint(x) {
            return this.paths[0].points
                .map((point, index) => ({
                    x: point.x,
                    diff: Math.abs(point.x - x),
                    index
                }))
                .reduce((memo, val) => (memo.diff < val.diff ? memo : val));
        }
    }
};
</script>

<style scoped>
.thick {
    stroke-width: 4px;
}

.thin {
    stroke-width: 1px;
}
.line,
.selector {
    fill: none;
    stroke-width: 3px;
}
.line-silver,
.line,
.selector {
    stroke: silver;
    background-color: silver;
}

.line-red {
    stroke: red;
    background-color: red;
}

.line-blue {
    stroke: steelblue;
    background-color: steelblue;
}
.line-orange {
    stroke: orange;
    background-color: orange;
}
.line-green {
    stroke: green;
    background-color: green;
}
.line-lightblue {
    stroke: lightblue;
    background-color: lightblue;
}
.line-magenta {
    stroke: magenta;
    background-color: magenta;
    stroke-width: 1px;
}
.line-violet {
    stroke: blueviolet;
    background-color: blueviolet;
}

.info {
    position: absolute;
    min-width: 10px;
    min-height: 10px;
    border: 1px solid red;
}
</style>

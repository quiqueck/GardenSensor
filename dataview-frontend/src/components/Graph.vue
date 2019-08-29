<template>
    <div style="width:100%;position:relative" ref="container">
        <svg
            ref="svg"
            @mousemove="mouseover"
            @mousedown="mousedown"
            @mouseup="mouseup"
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

        <div
            ref="stats"
            class="positionable info"
            :style="calcPosition()"
            v-if="popover.visible"
        >
            <ul>
                <li
                    v-for="item in popover.lines"
                    :key="item.nr"
                    :class="item.class"
                >
                    <nobr
                        ><label v-if="item.label !== undefined"
                            >{{ item.label }}:</label
                        >
                        <div id="value">
                            {{ item.value }}&thinsp;{{ item.unit }}
                        </div></nobr
                    >
                </li>
            </ul>
        </div>
    </div>
</template>

<script>
import * as d3 from "d3";
import { MomentDateFull } from "../filters.js";

export default {
    name: "LineGraph",
    data: function() {
        return {
            areaWidth: 200,
            paths: [{ line: [], points: [], id: 0 }],
            scaled: { x: null, y: null },
            selector: null,
            lastHoverPoint: {},
            lastMouse: { x: -1, y: -1, e: null },
            popover: {
                visible: false,
                lastUpdateVisible: false,
                left: 0,
                top: 0,
                lines: []
            }
        };
    },
    props: {
        values: {
            type: Object,
            default: () => {
                return {
                    id: -1,
                    data: [{ label: "", class: "line", values: [] }]
                };
            }
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
        data() {
            return this.values.data;
        },
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
            //console.log("Changed Size", this.areaWidth, this.height);
            this.update();
        },
        values: function(/*newValue, oldValue*/) {
            //console.log("Watched Data change", oldValue, "=>", newValue);
            this.update();
        }
    },
    updated() {
        if (this.popover.lastUpdateVisible != this.popover.visible) {
            this.mouseover(this.lastMouse.e);
        }
    },
    methods: {
        calcPosition() {
            return {
                top: this.popover.top + "px",
                left: this.popover.left + "px"
            };
        },
        onResize() {
            // console.log(
            //     "OnResize",
            //     this.areaWidth,
            //     this.$el.offsetWidth,
            //     this.$el
            // );
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
            //console.log("Updating", this.data.length);
            let updatedPaths = [];
            let mainDateScale = null;
            this.data.forEach(dataElement => {
                let updatedPath = {
                    id: dataElement.label,
                    label: dataElement.label,
                    class: "line " + dataElement.class,
                    color: dataElement.class,
                    unit: dataElement.unit,
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
                //console.log("Update", this.data);
                updatedPath.scaled.x.domain(mainDateScale);
                updatedPath.scaled.y.domain([
                    dataElement.unit == "%"
                        ? 0
                        : d3.min(dataElement.values, d => d.value),
                    dataElement.unit == "%"
                        ? 100
                        : d3.max(dataElement.values, d => d.value)
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
                //console.log("In", this.$refs.svg);
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
                    .attr("class", "axis blue-grey-6")
                    .call(d3.axisBottom(mainPath.scaled.x));

                // Add the Y0 Axis
                svg.append("g")
                    .attr(this.$options._scopeId, "")
                    .attr("id", "Axis")
                    .attr(
                        "style",
                        "transform:translate(" +
                            this.margin.left +
                            "px, " +
                            this.margin.top +
                            "px)"
                    )
                    .attr("class", "axis " + mainPath.color)
                    .call(d3.axisLeft(mainPath.scaled.y));
            }
        },
        mousedown({ offsetX, pageY, screenX, screenY }) {
            this.lastMouse.e = { offsetX, pageY, screenX, screenY };
            this.popover.visible = true;
        },
        mouseup() {
            this.popover.visible = false;
        },
        mouseover({ offsetX, pageY, screenX, screenY }) {
            if (
                Math.abs(screenX - this.lastMouse.x) < 2 &&
                Math.abs(screenY - this.lastMouse.y) < 2
            ) {
                return;
            }

            let lines = [];
            this.paths.forEach((path, idx) => {
                if (path.points.length > 0) {
                    const x = offsetX - this.margin.left;
                    const closestPoint = this.getClosestPoint(x, idx);
                    const point = path.points[closestPoint.index];

                    if (idx == 0) {
                        this.selector = this.createValueSelector([point]);
                        this.$emit(
                            "select",
                            this.data[0].values[closestPoint.index]
                        );

                        lines.push({
                            label: undefined,
                            value: MomentDateFull(closestPoint.src.date),
                            nr: -1,
                            class: "headline"
                        });
                    }
                    this.lastHoverPoint = closestPoint;

                    lines.push({
                        label: path.label,
                        value: closestPoint.src.value,
                        unit: path.unit,
                        nr: idx
                    });
                }
            });

            this.popover.lines = lines;
            this.popover.lastUpdateVisible = this.popover.visible;
            this.lastMouse.e = { offsetX, pageY, screenX, screenY };
            if (!this.popover.visible || this.$refs.stats === undefined) return;

            this.lastMouse.x = screenX;
            this.lastMouse.y = screenY;

            const container = this.$refs.svg.getBoundingClientRect();
            const box = this.$refs.stats.getBoundingClientRect();

            this.popover.left = Math.min(
                this.lastHoverPoint.x + this.margin.left + 2,
                container.width - box.width
            );
            this.popover.top = Math.max(0, pageY - container.y - box.height);
        },
        getClosestPoint(x, idx) {
            return this.paths[idx].points
                .map((point, index) => ({
                    src: point.val,
                    x: point.x,
                    diff: Math.abs(point.x - x),
                    index
                }))
                .reduce((memo, val) => (memo.diff < val.diff ? memo : val));
        }
    }
};
</script>

<style lang="stylus">
quasarcolors = '.red' $red, '.red-1' $red-1, '.red-2' $red-2, '.red-3' $red-3, '.red-4' $red-4, '.red-5' $red-5, '.red-6' $red-6, '.red-7' $red-7, '.red-8' $red-8, '.red-9' $red-9, '.red-10' $red-10, '.red-11' $red-11, '.red-12' $red-12, '.red-13' $red-13, '.red-14' $red-14, '.pink' $pink, '.pink-1' $pink-1, '.pink-2' $pink-2, '.pink-3' $pink-3, '.pink-4' $pink-4, '.pink-5' $pink-5, '.pink-6' $pink-6, '.pink-7' $pink-7, '.pink-8' $pink-8, '.pink-9' $pink-9, '.pink-10' $pink-10, '.pink-11' $pink-11, '.pink-12' $pink-12, '.pink-13' $pink-13, '.pink-14' $pink-14, '.purple' $purple, '.purple-1' $purple-1, '.purple-2' $purple-2, '.purple-3' $purple-3, '.purple-4' $purple-4, '.purple-5' $purple-5, '.purple-6' $purple-6, '.purple-7' $purple-7, '.purple-8' $purple-8, '.purple-9' $purple-9, '.purple-10' $purple-10, '.purple-11' $purple-11, '.purple-12' $purple-12, '.purple-13' $purple-13, '.purple-14' $purple-14, '.deep-purple' $deep-purple, '.deep-purple-1' $deep-purple-1, '.deep-purple-2' $deep-purple-2, '.deep-purple-3' $deep-purple-3, '.deep-purple-4' $deep-purple-4, '.deep-purple-5' $deep-purple-5, '.deep-purple-6' $deep-purple-6, '.deep-purple-7' $deep-purple-7, '.deep-purple-8' $deep-purple-8, '.deep-purple-9' $deep-purple-9, '.deep-purple-10' $deep-purple-10, '.deep-purple-11' $deep-purple-11, '.deep-purple-12' $deep-purple-12, '.deep-purple-13' $deep-purple-13, '.deep-purple-14' $deep-purple-14, '.indigo' $indigo, '.indigo-1' $indigo-1, '.indigo-2' $indigo-2, '.indigo-3' $indigo-3, '.indigo-4' $indigo-4, '.indigo-5' $indigo-5, '.indigo-6' $indigo-6, '.indigo-7' $indigo-7, '.indigo-8' $indigo-8, '.indigo-9' $indigo-9, '.indigo-10' $indigo-10, '.indigo-11' $indigo-11, '.indigo-12' $indigo-12, '.indigo-13' $indigo-13, '.indigo-14' $indigo-14, '.blue' $blue, '.blue-1' $blue-1, '.blue-2' $blue-2, '.blue-3' $blue-3, '.blue-4' $blue-4, '.blue-5' $blue-5, '.blue-6' $blue-6, '.blue-7' $blue-7, '.blue-8' $blue-8, '.blue-9' $blue-9, '.blue-10' $blue-10, '.blue-11' $blue-11, '.blue-12' $blue-12, '.blue-13' $blue-13, '.blue-14' $blue-14, '.light-blue' $light-blue, '.light-blue-1' $light-blue-1, '.light-blue-2' $light-blue-2, '.light-blue-3' $light-blue-3, '.light-blue-4' $light-blue-4, '.light-blue-5' $light-blue-5, '.light-blue-6' $light-blue-6, '.light-blue-7' $light-blue-7, '.light-blue-8' $light-blue-8, '.light-blue-9' $light-blue-9, '.light-blue-10' $light-blue-10, '.light-blue-11' $light-blue-11, '.light-blue-12' $light-blue-12, '.light-blue-13' $light-blue-13, '.light-blue-14' $light-blue-14, '.cyan' $cyan, '.cyan-1' $cyan-1, '.cyan-2' $cyan-2, '.cyan-3' $cyan-3, '.cyan-4' $cyan-4, '.cyan-5' $cyan-5, '.cyan-6' $cyan-6, '.cyan-7' $cyan-7, '.cyan-8' $cyan-8, '.cyan-9' $cyan-9, '.cyan-10' $cyan-10, '.cyan-11' $cyan-11, '.cyan-12' $cyan-12, '.cyan-13' $cyan-13, '.cyan-14' $cyan-14, '.teal' $teal, '.teal-1' $teal-1, '.teal-2' $teal-2, '.teal-3' $teal-3, '.teal-4' $teal-4, '.teal-5' $teal-5, '.teal-6' $teal-6, '.teal-7' $teal-7, '.teal-8' $teal-8, '.teal-9' $teal-9, '.teal-10' $teal-10, '.teal-11' $teal-11, '.teal-12' $teal-12, '.teal-13' $teal-13, '.teal-14' $teal-14, '.green' $green, '.green-1' $green-1, '.green-2' $green-2, '.green-3' $green-3, '.green-4' $green-4, '.green-5' $green-5, '.green-6' $green-6, '.green-7' $green-7, '.green-8' $green-8, '.green-9' $green-9, '.green-10' $green-10, '.green-11' $green-11, '.green-12' $green-12, '.green-13' $green-13, '.green-14' $green-14, '.light-green' $light-green, '.light-green-1' $light-green-1, '.light-green-2' $light-green-2, '.light-green-3' $light-green-3, '.light-green-4' $light-green-4, '.light-green-5' $light-green-5, '.light-green-6' $light-green-6, '.light-green-7' $light-green-7, '.light-green-8' $light-green-8, '.light-green-9' $light-green-9, '.light-green-10' $light-green-10, '.light-green-11' $light-green-11, '.light-green-12' $light-green-12, '.light-green-13' $light-green-13, '.light-green-14' $light-green-14, '.lime' $lime, '.lime-1' $lime-1, '.lime-2' $lime-2, '.lime-3' $lime-3, '.lime-4' $lime-4, '.lime-5' $lime-5, '.lime-6' $lime-6, '.lime-7' $lime-7, '.lime-8' $lime-8, '.lime-9' $lime-9, '.lime-10' $lime-10, '.lime-11' $lime-11, '.lime-12' $lime-12, '.lime-13' $lime-13, '.lime-14' $lime-14, '.yellow' $yellow, '.yellow-1' $yellow-1, '.yellow-2' $yellow-2, '.yellow-3' $yellow-3, '.yellow-4' $yellow-4, '.yellow-5' $yellow-5, '.yellow-6' $yellow-6, '.yellow-7' $yellow-7, '.yellow-8' $yellow-8, '.yellow-9' $yellow-9, '.yellow-10' $yellow-10, '.yellow-11' $yellow-11, '.yellow-12' $yellow-12, '.yellow-13' $yellow-13, '.yellow-14' $yellow-14, '.amber' $amber, '.amber-1' $amber-1, '.amber-2' $amber-2, '.amber-3' $amber-3, '.amber-4' $amber-4, '.amber-5' $amber-5, '.amber-6' $amber-6, '.amber-7' $amber-7, '.amber-8' $amber-8, '.amber-9' $amber-9, '.amber-10' $amber-10, '.amber-11' $amber-11, '.amber-12' $amber-12, '.amber-13' $amber-13, '.amber-14' $amber-14, '.orange' $orange, '.orange-1' $orange-1, '.orange-2' $orange-2, '.orange-3' $orange-3, '.orange-4' $orange-4, '.orange-5' $orange-5, '.orange-6' $orange-6, '.orange-7' $orange-7, '.orange-8' $orange-8, '.orange-9' $orange-9, '.orange-10' $orange-10, '.orange-11' $orange-11, '.orange-12' $orange-12, '.orange-13' $orange-13, '.orange-14' $orange-14, '.deep-orange' $deep-orange, '.deep-orange-1' $deep-orange-1, '.deep-orange-2' $deep-orange-2, '.deep-orange-3' $deep-orange-3, '.deep-orange-4' $deep-orange-4, '.deep-orange-5' $deep-orange-5, '.deep-orange-6' $deep-orange-6, '.deep-orange-7' $deep-orange-7, '.deep-orange-8' $deep-orange-8, '.deep-orange-9' $deep-orange-9, '.deep-orange-10' $deep-orange-10, '.deep-orange-11' $deep-orange-11, '.deep-orange-12' $deep-orange-12, '.deep-orange-13' $deep-orange-13, '.deep-orange-14' $deep-orange-14, '.brown' $brown, '.brown-1' $brown-1, '.brown-2' $brown-2, '.brown-3' $brown-3, '.brown-4' $brown-4, '.brown-5' $brown-5, '.brown-6' $brown-6, '.brown-7' $brown-7, '.brown-8' $brown-8, '.brown-9' $brown-9, '.brown-10' $brown-10, '.brown-11' $brown-11, '.brown-12' $brown-12, '.brown-13' $brown-13, '.brown-14' $brown-14, '.grey' $grey, '.grey-1' $grey-1, '.grey-2' $grey-2, '.grey-3' $grey-3, '.grey-4' $grey-4, '.grey-5' $grey-5, '.grey-6' $grey-6, '.grey-7' $grey-7, '.grey-8' $grey-8, '.grey-9' $grey-9, '.grey-10' $grey-10, '.grey-11' $grey-11, '.grey-12' $grey-12, '.grey-13' $grey-13, '.grey-14' $grey-14, '.blue-grey' $blue-grey, '.blue-grey-1' $blue-grey-1, '.blue-grey-2' $blue-grey-2, '.blue-grey-3' $blue-grey-3, '.blue-grey-4' $blue-grey-4, '.blue-grey-5' $blue-grey-5, '.blue-grey-6' $blue-grey-6, '.blue-grey-7' $blue-grey-7, '.blue-grey-8' $blue-grey-8, '.blue-grey-9' $blue-grey-9, '.blue-grey-10' $blue-grey-10, '.blue-grey-11' $blue-grey-11, '.blue-grey-12' $blue-grey-12, '.blue-grey-13' $blue-grey-13, '.blue-grey-14' $blue-grey-14;

for cl in quasarcolors {
    {replace('\.', '.line.', cl)} {
        background-color: cl[1][1];
        stroke: cl[1];
    }

    {replace('\.', '.axis.', cl)} {
        line, path {
            stroke: cl[1];
        }

        text {
            fill: cl[1];
        }
    }
}

.thick {
    stroke-width: 4px !important;
}

.thin {
    stroke-width: 1px !important;
}

.line, .selector {
    fill: none;
    stroke-width: 3px;
}

.selector {
    @extend .thin;
    @extend .line.grey-8;
    opacity: 0.5;
}

.hairline {
    @extend .thin;
    @extend .line.grey-8;
    opacity: 0.5;
}

.info {
    min-width: 10px;
    min-height: 10px;
    background-color: white;
    border: 1px dashed $grey-6;
    opacity: 0.95;
}

.positionable {
    position: absolute;
    pointer-events: none;
}

ul {
    list-style-type: none;
    padding: 0px;
    margin: 8px;

    .headline {
        color: $grey-8;
        font-weight: bold;
        font-size: 110%;
    }

    li {
        font-size: 90%;
        color: $grey-6;

        label {
            display: inline-block;
            margin-left: 20px !important;
            font-weight: bold;
        }
    }
}

#value {
    display: inline-block;
    padding-left: 10px;
}
</style>

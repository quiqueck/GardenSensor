import moment from "moment";

export const MomentDate = function(d) {
  if (d === undefined) {
    return "---";
  }
  if (
    moment()
      .subtract(1, "hours")
      .isBefore(d)
  ) {
    return moment(d)
      .locale("de")
      .fromNow();
  } else {
    return moment(d)
      .locale("de")
      .calendar();
  }
};

export const MomentDateFull = function(d) {
  if (d === undefined) {
    return "---";
  }
  if (
    moment()
      .subtract(1, "hours")
      .isBefore(d)
  ) {
    return moment(d)
      .locale("de")
      .format("LLLL");
  } else {
    return moment(d)
      .locale("de")
      .format("LLLL");
  }
};

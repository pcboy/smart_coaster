$fn = 40;
module coaster_bottom(width, depth, height) {
  minkowski() {
   sphere(1);
    difference() {
      cube([width, depth, 10], center = true);

      translate([0, 0, 5]) {
        color("red") cube([width - 6, depth - 6, 10], center = true);
      }
    }
  }
}

coaster_bottom(117,117,10);

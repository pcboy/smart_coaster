$fn = 40;
esp_width = 23;
esp_height = 7.5;
esp_depth = 51.5;

module coaster_bottom(width, depth, height) {
  color("white") minkowski() {
    sphere(1);
    difference() {
      cube([width, depth, 10], center = true);

      translate([0, 0, 5]) {
        color("red") cube([width - 6, depth - 6, 10], center = true);
      }
    }
  }
}

module sensor_holder(width, depth, height) {
  color("red") difference() {
    cube([width + 2, depth + 2, height], center = true);
    translate([0,0,5]) {
      cube([width, depth, height], center = true);
    }
  }
}

coaster_bottom(117,117,10);
sensor_holder(77, 77, 12);

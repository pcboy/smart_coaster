esp_width = 23;
esp_height = 7.5;
esp_depth = 51.5;

sensor_width = 77;
sensor_height = 12;
coaster_width = sensor_width + esp_width;
coaster_top_height = 6;
coaster_bottom_height = sensor_height;

module coaster_bottom(width, depth, height) {
  color("white") {
    difference() {
      cube([width, depth, height], center = true);

      translate([0, 0, 5]) {
        color("red") cube([width - 6, depth - 6, height], center = true);
      }
    }
  }
}

module coaster_top(width, depth, height) {
  rotate(a = [180, 0, 0]) {
    translate([0,0, -12]) {
      color("green") coaster_bottom(width, depth, height);
    }
  }
}

module coaster_top_support(size) {
  translate([-size / 2, 0, 0]) {
    union() {
      cube([size, 3, coaster_top_height + 2]);
      translate([size / 2, - size / 2, 0]) {
        cube([3, size, coaster_top_height + 2]);
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

module snap_fit(width, depth, height, x, y, z) {
  translate([x - width + 2, y - depth, z + height]) {
    rotate(a = [90, 180, 0]) {
      color("purple")
        scale([1.2,1,1])
        linear_extrude(depth, center = true)
        polygon(points=[[0,0], [0, height - 2], [-2, height - 2],[-2, height - 1], [-1, height],[1, height], [1,0]]);
    }
  }
}

#coaster_bottom(coaster_width, coaster_width, sensor_height);
sensor_holder(sensor_width, sensor_width, sensor_height);
#coaster_top(coaster_width, coaster_width, coaster_top_height);
coaster_top_support(sensor_width);

snap_fit(2, 3, 
    coaster_top_height / 2 + coaster_bottom_height / 2,
    coaster_width / 2 - 4,
    coaster_width / 2 - 4.5, 
    coaster_bottom_height - 10);

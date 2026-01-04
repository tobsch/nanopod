// NanoPod Oak Stand
// Parametric design for CrowPanel 1.28" ESP32 Display
// Material: Oak wood

/* [Main Dimensions] */
width = 120;          // mm - total width
depth = 80;           // mm - total depth
front_height = 15;    // mm - height at front edge
angle = 15;           // degrees - viewing angle

/* [Device Pocket] */
pocket_diameter = 55; // mm - diameter for device
pocket_depth = 18;    // mm - depth of pocket
cable_hole = 6;       // mm - through-hole for cable

/* [Finishing] */
bevel_size = 1.5;     // mm - edge chamfer size

/* [Calculated] */
back_height = front_height + depth * tan(angle);

// Main wedge body
module wedge_body() {
    hull() {
        // Front edge - thin rectangle
        translate([0, 0, 0])
            cube([width, 1, front_height]);

        // Back edge - tall rectangle
        translate([0, depth - 1, 0])
            cube([width, 1, back_height]);
    }
}

// Beveled wedge body
module beveled_wedge() {
    minkowski() {
        // Slightly smaller body
        translate([bevel_size, bevel_size, bevel_size])
        scale([(width - 2*bevel_size)/width,
               (depth - 2*bevel_size)/depth,
               1])
        difference() {
            wedge_body();
            // Remove top to prevent double bevel
            translate([-1, -1, back_height])
                cube([width + 2, depth + 2, bevel_size * 2]);
        }
        // Bevel sphere
        sphere(r = bevel_size, $fn = 16);
    }
}

// Device pocket (cylinder cut into angled face)
module device_pocket() {
    // Center of top angled surface
    center_x = width / 2;
    center_y = depth / 2;
    surface_z = front_height + center_y * tan(angle);

    // 55mm dia, 18mm deep, perpendicular to 15° top surface
    translate([center_x, center_y, surface_z])
    rotate([angle, 0, 0])  // Tilt 15° to match surface angle
    translate([0, 0, -pocket_depth])
    cylinder(h = pocket_depth + 1, d = pocket_diameter, $fn = 64);
}

// Cable through-hole (from center of pocket straight to back)
module cable_hole() {
    center_x = width / 2;
    center_y = depth / 2;
    center_z = front_height + center_y * tan(angle);

    // Drill from center of pocket straight to back of case
    translate([center_x, center_y, center_z])
    rotate([-90, 0, 0])
    cylinder(h = depth, d = cable_hole, $fn = 32);
}

// Final stand assembly
module stand() {
    // Surface height at center
    surface_z = front_height + (depth/2) * tan(angle);

    difference() {
        wedge_body();
        // 55mm pocket, 18mm deep, perpendicular to 15° surface
        translate([60, 40, surface_z])
        rotate([angle, 0, 0])  // Perpendicular to angled top surface
        translate([0, 0, -pocket_depth])
        cylinder(h = pocket_depth + 1, d = pocket_diameter, $fn = 64);

        // 6mm cable hole to back
        translate([60, 40, surface_z])
        rotate([-90, 0, 0])
        cylinder(h = depth, d = cable_hole, $fn = 32);
    }
}

// Render the stand
stand();

// Optional: Show device position (uncomment to visualize)
// %translate([width/2, depth/2, front_height + (depth/2) * tan(angle)])
//   rotate([-angle, 0, 0])
//   cylinder(h = 20, d = 52, $fn = 64);

echo("=== NanoPod Stand Specifications ===");
echo(str("Width: ", width, " mm"));
echo(str("Depth: ", depth, " mm"));
echo(str("Front height: ", front_height, " mm"));
echo(str("Back height: ", back_height, " mm"));
echo(str("Angle: ", angle, " degrees"));
echo(str("Pocket: ", pocket_diameter, " mm dia x ", pocket_depth, " mm deep"));
echo(str("Cable hole: ", cable_hole, " mm"));

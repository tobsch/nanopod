// NanoPod Oak Stand
// Parametric design for CrowPanel 1.28" ESP32 Display
// Material: Oak wood

/* [Main Dimensions] */
width = 80;           // mm - total width
depth = 80;           // mm - total depth
front_height = 15;    // mm - height at front edge
angle = 15;           // degrees - viewing angle

/* [Device Pocket] */
pocket_diameter = 55; // mm - diameter for device
pocket_depth = 18;    // mm - depth of pocket
cable_hole = 6;       // mm - through-hole for cable

/* [Finishing] */
bevel_size = 1;       // mm - edge chamfer size

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

// Chamfered wedge body (1mm edgy bevels on all long edges)
module beveled_wedge() {
    c = bevel_size;

    difference() {
        wedge_body();

        // === BOTTOM EDGES ===
        // Bottom front edge
        translate([0, 0, 0])
        rotate([45, 0, 0])
        translate([-1, -c, -c])
        cube([width + 2, c * 2, c * 2]);

        // Bottom back edge
        translate([0, depth, 0])
        rotate([45, 0, 0])
        translate([-1, -c, -c])
        cube([width + 2, c * 2, c * 2]);

        // Bottom left edge
        translate([0, 0, 0])
        rotate([0, -45, 0])
        translate([-c, -1, -c])
        cube([c * 2, depth + 2, c * 2]);

        // Bottom right edge
        translate([width, 0, 0])
        rotate([0, -45, 0])
        translate([-c, -1, -c])
        cube([c * 2, depth + 2, c * 2]);

        // === VERTICAL CORNER EDGES ===
        // Front left vertical
        translate([0, 0, 0])
        rotate([0, 0, 45])
        translate([-c * 0.7, -c * 0.7, -1])
        cube([c * 1.4, c * 1.4, front_height + 2]);

        // Front right vertical
        translate([width, 0, 0])
        rotate([0, 0, 45])
        translate([-c * 0.7, -c * 0.7, -1])
        cube([c * 1.4, c * 1.4, front_height + 2]);

        // Back left vertical
        translate([0, depth, 0])
        rotate([0, 0, 45])
        translate([-c * 0.7, -c * 0.7, -1])
        cube([c * 1.4, c * 1.4, back_height + 2]);

        // Back right vertical
        translate([width, depth, 0])
        rotate([0, 0, 45])
        translate([-c * 0.7, -c * 0.7, -1])
        cube([c * 1.4, c * 1.4, back_height + 2]);

        // === TOP EDGES (along angled surface) ===
        // Top left edge (along slope)
        translate([0, 0, front_height])
        rotate([angle, 0, 0])
        rotate([0, -45, 0])
        translate([-c * 0.7, -1, -c * 0.7])
        cube([c * 1.4, depth / cos(angle) + 2, c * 1.4]);

        // Top right edge (along slope)
        translate([width, 0, front_height])
        rotate([angle, 0, 0])
        rotate([0, -45, 0])
        translate([-c * 0.7, -1, -c * 0.7])
        cube([c * 1.4, depth / cos(angle) + 2, c * 1.4]);
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
        beveled_wedge();  // Slightly rounded edges
        // 55mm pocket, 18mm deep, perpendicular to 15° surface
        translate([width/2, depth/2, surface_z])
        rotate([angle, 0, 0])  // Perpendicular to angled top surface
        translate([0, 0, -pocket_depth])
        cylinder(h = pocket_depth + 1, d = pocket_diameter, $fn = 64);

        // Bevel on top edge of pocket (1mm chamfer)
        translate([width/2, depth/2, surface_z])
        rotate([angle, 0, 0])
        translate([0, 0, -bevel_size])
        cylinder(h = bevel_size + 1, d1 = pocket_diameter, d2 = pocket_diameter + bevel_size * 2, $fn = 64);
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

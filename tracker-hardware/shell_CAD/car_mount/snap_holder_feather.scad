snap_holder_feather();
cube([50, 6, 3], center=true);
translate([0,0,0])
cube([6, 23, 3], center=true);

module snap_holder_feather(){
    translate([-3,-23/2-3,-1.5])
    snap_arm();
    translate([5,+23/2+3,-1.5])
    rotate([0,0,180])
    snap_arm(length=10);
    /*
    translate([107/2+3,-3,-1.5])
    rotate([0,0,90])
    snap_arm();
    
    translate([-107/2-3,3,-1.5])
    rotate([0,0,-90])
    snap_arm();
    */
}

module snap_arm(stand_height=10, lip_height=3, length=6){
    cube([length,6,stand_height]);
    cube([length,3,stand_height+lip_height+2]);
    translate([0,0,stand_height+lip_height])
    rotate([90,0,90])
    linear_extrude(length)
    polygon([[0,0], [0,10], [4,0]]);
}
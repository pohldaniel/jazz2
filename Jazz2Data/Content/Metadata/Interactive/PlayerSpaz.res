{
	"Version": {
		"Target": "Jazz² Resurrection"
	},

	"Animations": {
		"Idle": {
			"Path": "Spaz/idle.aura",
			"States": [ 0 ]
		},
		"Walk": {
			"Path": "Spaz/run.aura",
			"States": [ 1, 17 ]
		},
		"Run": {
			"Path": "Spaz/dash_start.aura",
			"States": [ 2, 18 ]
		},
		"Jump": {
			"Path": "Spaz/jump.aura",
			"States": [ 4 ]
		},
		"RunJump": {
			"Path": "Spaz/jump_diag.aura",
			"Flags": 1,
			"States": [ 5, 6 ]
		},
		"Fall": {
			"Path": "Spaz/fall.aura",
			"States": [ 8 ]
		},
		"FallDiag": {
			"Path": "Spaz/fall_diag.aura",
			"States": [ 9, 10, 11 ]
		},
		"Freefall": {
			"Path": "Spaz/freefall.aura",
			"States": [ 65536, 65537, 65538, 65539 ]
		},
		"Dash": {
			"Path": "Spaz/dash.aura",
			"FrameRate": 20,
			"States": [ 3, 19 ]
		},
		"DashJump": {
			"Path": "Spaz/ball.aura",
			"States": [ 7 ]
		},
		"Lookup": {
			"Path": "Spaz/lookup_start.aura",
			"Flags": 1,
			"FrameRate": 34,
			"States": [ 64 ]
		},
		"Crouch": {
			"Path": "Spaz/crouch_start.aura",
			"Flags": 1,
			"FrameRate": 38,
			"States": [ 32 ]
		},
		"DizzyIdle": {
			"Path": "Spaz/dizzy.aura",
			"FrameRate": 7,
			"States": [ 128 ]
		},
		"DizzyWalk": {
			"Path": "Spaz/dizzy_walk.aura",
			"FrameRate": 6,
			"States": [ 129, 145 ]
		},
		"Shoot": {
			"Path": "Spaz/shoot.aura",
			"States": [ 16, 144 ],
			"Flags": 1
		},
		"CrouchShoot": {
			"Path": "Spaz/crouch_shoot.aura",
			"States": [ 48 ],
			"Flags": 1
		},
		"LookupShoot": {
			"Path": "Spaz/shoot_ver.aura",
			"States": [ 80 ],
			"Flags": 1
		},
		"Hurt": {
			"Path": "Spaz/hurt.aura",
			"FrameRate": 6,
			"States": [ 2048 ]
		},
		"Sidekick": {
			"Path": "Spaz/sidekick.aura",
			"States": [ 512 ]
		},
		"Buttstomp": {
			"Path": "Spaz/buttstomp.aura",
			"States": [ 256 ]
		},
		"Hook": {
			"Path": "Spaz/vine_idle.aura",
			"States": [ 12 ]
		},
		"HookLookup": {
			"Path": "Spaz/vine_shoot_up_end.aura",
			"States": [ 76 ]
		},
		"HookMove": {
			"Path": "Spaz/vine_walk.aura",
			"States": [ 13, 14, 15 ]
		},
		"Copter": {
			"Path": "Spaz/copter.aura",
			"FrameRate": 8,
			"States": [ 8192, 8193, 8194, 8195 ]
		},
		"CopterShoot": {
			"Path": "Spaz/copter_shoot.aura",
			"States": [ 8208, 8209, 8210, 8211 ]
		},
		"AerialShoot": {
			"Path": "Spaz/fall_shoot.aura",
			"FrameRate": 14,
			"States": [ 20, 21, 22, 23, 24, 25, 26, 27, 262160, 262161, 262162, 262163 ]
		},
		"HookShoot": {
			"Path": "Spaz/vine_shoot.aura",
			"States": [ 28, 29, 30, 31 ],
			"Flags": 1
		},
		"HookLookupShoot": {
			"Path": "Spaz/vine_shoot_up.aura",
			"States": [ 92 ],
			"Flags": 1
		},
		"RunToIdle": {
			"Path": "Spaz/run_stop.aura",
			"States": [ 1073741824 ]
		},
		"RunToDash": {
			"Path": "Spaz/dash_start.aura",
			"States": [ 1073741825 ]
		},
		"DashToIdle": {
			"Path": "Spaz/dash_stop.aura",
			"States": [ 1073741856 ]
		},
		"FallToIdle": {
			"Path": "Spaz/fall_end.aura",
			"States": [ 1073741826 ]
		},
		"ShootToIdle": {
			"Path": "Spaz/shoot_start.aura",
			"FrameRate": 20,
			"States": [ 1073741828 ]
		},
		"HookShootToHook": {
			"Path": "Spaz/vine_shoot_end.aura",
			"FrameRate": 24,
			"States": [ 1073741829 ]
		},
		"CopterShootToCopter": {
			"Path": "Spaz/copter_shoot_start.aura",
			"FrameRate": 24,
			"States": [ 1073741830 ]
		},
		"FallShootToFall": {
			"Path": "Spaz/jump_shoot_end.aura",
			"FrameRate": 20,
			"States": [ 1073741872 ]
		},
		"SidekickA": {
			"Path": "Spaz/sidekick_start.aura",
			"FrameCount": 5,
			"FrameRate": 20,
			"States": [ 1073741831 ]
		},
		"SidekickB": {
			"Path": "Spaz/sidekick_start.aura",
			"FrameOffset": 5,
			"FrameRate": 20,
			"States": [ 1073741832 ]
		},
		"SidekickC": {
			"Path": "Spaz/sidekick_end.aura",
			"FrameRate": 30,
			"States": [ 1073741833 ]
		},
		"ButtstompStart": {
			"Path": "Spaz/spring.aura",
			"States": [ 1073741834 ]
		},
		"ButtstompEnd": {
			"Path": "Spaz/buttstomp_end.aura",
			"States": [ 1073741858 ]
		},
		"PoleH": {
			"Path": "Spaz/pole_h.aura",
			"FrameRate": 20,
			"States": [ 1073741835 ]
		},
		"PoleHSlow": {
			"Path": "Spaz/pole_h.aura",
			"States": [ 1073741837 ]
		},
		"PoleV": {
			"Path": "Spaz/pole_v.aura",
			"FrameRate": 20,
			"States": [ 1073741836 ]
		},
		"PoleVSlow": {
			"Path": "Spaz/pole_v.aura",
			"States": [ 1073741838 ]
		},
		"Death": {
			"Path": "Spaz/die.aura",
			"FrameRate": 3,
			"States": [ 1073741839 ]
		},
		"WarpIn": {
			"Path": "Spaz/warp_in.aura",
			"States": [ 1073741843 ]
		},
		"WarpOut": {
			"Path": "Spaz/warp_out.aura",
			"States": [ 1073741844 ]
		},
		"WarpInFreefall": {
			"Path": "Spaz/warp_in_freefall.aura",
			"States": [ 1073741847 ]
		},
		"WarpOutFreefall": {
			"Path": "Spaz/warp_out_freefall.aura",
			"States": [ 1073741848 ]
		},
		"Spring": {
			"Path": "Spaz/spring.aura",
			"States": [ 262144, 262145, 262146, 262147 ]
		},
		"Push": {
			"Path": "Spaz/push.aura",
			"FrameRate": 7,
			"States": [ 16384, 16385 ]
		},
		"EndOfLevel": {
			"Path": "Spaz/eol.aura",
			"FrameRate": 3,
			"States": [ 1073741846 ]
		},
		"Swim": {
			"Path": "Spaz/swim_right.aura",
			"FrameRate": 6,
			"States": [ 4096 ]
		},
		"Lift": {
			"Path": "Spaz/lift.aura",
			"Flags": 1,
			"FrameRate": 16,
			"States": [ 131072 ]
		},
		"LiftStart": {
			"Path": "Spaz/lift_jump_heavy.aura",
			"FrameRate": 16,
			"States": [ 1073741859 ]
		},
		"LiftEnd": {
			"Path": "Spaz/lift_jump_light.aura",
			"States": [ 1073741860 ]
		},
		"Ledge": {
			"Path": "Spaz/ledge.aura",
			"FrameRate": 3,
			"States": [ 1073741861 ]
		},
		"Airboard": {
			"Path": "Spaz/airboard.aura",
			"FrameRate": 6,
			"States": [ 1024 ]
		},
		"LedgeClimb": {
			"Path": "Spaz/ledge_climb.aura",
			"FrameRate": 8,
			"States": [ 1073741862 ]
		},
		"Swing": {
			"Path": "Spaz/swing.aura",
			"FrameRate": 4,
			"States": [ 32768 ],
			"Flags": 1
		},
		
		"IdleBored1": {
			"Path": "Spaz/idle_flavor_2.aura",
			"FrameRate": 2
		},
		"IdleBored2": {
			"Path": "Spaz/idle_flavor_3.aura",
			"FrameRate": 1
		},
		"IdleBored3": {
			"Path": "Spaz/idle_flavor_4.aura",
			"FrameRate": 3
		},
		"IdleBored4": {
			"Path": "Spaz/idle_flavor_5.aura",
			"FrameRate": 3
		},

		"TransformFromFrog": {
			"Path": "Spaz/transform_frog_end.aura",
			"FrameRate": 3,
			"States": [ 1073741888 ]
		},
		
		"Corpse": {
			"Path": "Spaz/corpse.aura"
		},

		"SugarRush": {
			"Path": "Common/sugar_rush_stars.aura"
		},
		"Shield": {
			"Path": "Common/player_shield.aura"
		},
		"ShieldFire": {
			"Path": "Common/shield_fire.aura"
		},
		"ShieldWater": {
			"Path": "Common/shield_water.aura"
		},
		"ShieldLightning": {
			"Path": "Common/shield_lightning.aura"
		}
	},

	"Sounds": {
		"ChangeWeapon": {
			"Paths": [ "UI/weapon_change.wav" ]
		},
		"EndOfLevel": {
			"Paths": [ "Spaz/level_complete.wav" ]
		},
		"WarpIn": {
			"Paths": [ "Common/warp_in.wav" ]
		},
		"WarpOut": {
			"Paths": [ "Common/warp_out.wav" ]
		},
		"Jump": {
			"Paths": [ "Common/char_jump.wav" ]
		},
		"Land": {
			"Paths": [ "Common/char_land.wav" ]
		},
		"Hurt": {
			"Paths": [ "Spaz/hurt_1.wav", "Spaz/hurt_2.wav" ]
		},
		"Die": {
			"Paths": [ "Spaz/idle_flavor_4.wav" ]
		},
		"Copter": {
			"Paths": [ "Common/copter_noise.wav" ]
		},
		"PickupAmmo": {
			"Paths": [ "Pickup/ammo.wav" ]
		},
		"PickupCoin": {
			"Paths": [ "Pickup/coin.wav" ]
		},
		"PickupGem": {
			"Paths": [ "Pickup/gem.wav" ]
		},
		"PickupOneUp": {
			"Paths": [ "Pickup/1up.wav" ]
		},
		"PickupDrink": {
			"Paths": [ "Pickup/food_drink_1.wav", "Pickup/food_drink_2.wav", "Pickup/food_drink_3.wav", "Pickup/food_drink_4.wav" ]
		},
		"PickupFood": {
			"Paths": [ "Pickup/food_edible_1.wav", "Pickup/food_edible_2.wav", "Pickup/food_edible_3.wav", "Pickup/food_edible_4.wav" ]
		},
		"PickupMaxCarrot": {
			"Paths": [ "Pickup/food_edible_1.wav" ]
		},
		"WeaponBlaster": {
			"Paths": [ "Weapon/bullet_blaster_jazz_2.wav", "Weapon/bullet_blaster_jazz_3.wav" ]
		},
		"WeaponToaster": {
			"Paths": [ "Weapon/toaster.wav" ]
		},
		"WeaponThunderbolt": {
			"Paths": [ "Unknown/unknown_bonus1.wav" ]
		},
		"HookAttach": {
			"Paths": [ "Common/swish_9.wav" ]
		},
		"Buttstomp": {
			"Paths": [ "Common/down.wav" ]
		},
		"Buttstomp2": {
			"Paths": [ "Spaz/jump_3.wav" ]
		},
		"EndOfLevel1": {
			"Paths": [ "Common/char_revup.wav" ]
		},
		"EndOfLevel2": {
			"Paths": [ "Weapon/ricochet_bullet_3.wav" ]
		},
		"Spring": {
			"Paths": [ "Spaz/spring_1.wav", "Spaz/spring_2.wav" ]
		},
		"Pole": {
			"Paths": [ "Birdy/fly_1.wav", "Birdy/fly_2.wav" ]
		},
		"Ledge": {
			"Paths": [ "Spaz/ledge.wav" ]
		},
		"IdleBored1": {
			"Paths": [ "Spaz/idle_flavor_2.wav" ]
		},
		"DoubleJump": {
			"Paths": [ "Common/char_double_jump.wav" ]
		},
		"Sidekick": {
			"Paths": [ "Spaz/sidekick_1.wav", "Spaz/sidekick_2.wav" ]
		},
		"BonusWarpNotEnoughCoins": {
			"Paths": [ "Object/bonus_not_enough_coins.wav" ]
		}
	}
}
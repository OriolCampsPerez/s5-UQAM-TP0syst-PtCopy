#!/usr/bin/env bats

load test_helper


@test "multiple" {
	run ./pcopy tests/hello 0-18 tests/other/bonjour.java 0-104 tests/other/autre/hello 0-218
	check 0 "340"

}

@test "multiple-contenu" {
	run ls copies
	checki 0 <<FIN
bonjour.java
hello
FIN
}

@test "multiple-size" {
	run wc -c copies/hello
	check 0 "218 copies/hello"
}

@test "cas-limite" {
	run ./pcopy tests/hello 17-1
	check 0 "1"

}

@test "cas-limite-size" {
	run wc -c copies/hello
	check 0 "1 copies/hello"
}

@test "duplicate" {
	run ./pcopy tests/hello 0-18 tests/hello 0-17 tests/hello 0-16 tests/hello 0-15 tests/hello 0-14
	check 0 "80"
}

@test "duplicate-size" {
	run wc -c copies/hello
	check 0 "14 copies/hello"
}

@test "out-border" {
	run ./pcopy tests/hello 19-1
	check 1 "0"
}

@test "binaire" {
	run ./pcopy image.png 0-4095
	check 0 "4095"
}

@test "binaire-eq-content" {
	run cmp -n 4095 image.png copies/image.png
	check 0 ""
}

@test "binaire-neq-content" {
	run cmp -n 4096 image.png copies/image.png
	check 1 "cmp: EOF on copies/image.png after byte 4095, in line 9"
}

@test "urandom" {
	run ./pcopy /dev/urandom 1234567890-4095
	check 0 "4095"
}

@test "urandom-size" {
	run wc -c copies/urandom
	check 0 "4095 copies/urandom"
}

@test "accent" {
	run ./pcopy tests/accent 0-1
	check 0 "1"
}

@test "accent-size" {
	run wc -c copies/accent
	check 0 "1 copies/accent"
}

@test "accent-cmp" {
	run cmp tests/accent copies/accent
	check 1 "cmp: EOF on copies/accent after byte 1, in line 1"
}

@test "vide" {
	run ./pcopy tests/vide 0-1
	check 1 "0"
}

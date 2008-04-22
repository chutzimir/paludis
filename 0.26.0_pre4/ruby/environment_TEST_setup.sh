#!/bin/bash
# vim: set ft=sh sw=4 sts=4 et :

mkdir environment_TEST_dir || exit 1
cd environment_TEST_dir || exit 1

mkdir -p home/.paludis/repositories

cat <<END > home/.paludis/repositories/testrepo.conf
location = `pwd`/testrepo
format = ebuild
names_cache = /var/empty
cache = /var/empty
profiles = \${location}/profiles/testprofile
END

cat <<END > home/.paludis/keywords.conf
*/* test testtest testtesttest
~foo/bar-1.0 ~test
END

cat <<END > home/.paludis/use.conf
*/* enabled
~foo/bar-1.0 sometimes_enabled
END

cat <<END > home/.paludis/package_mask.conf
=foo/bar-3*
END

cat <<END > home/.paludis/package_unmask.conf
=foo/bar-2*
END

cat <<END > home/.paludis/licenses.conf
foo/bar GPL-2
foo/baz *
END

cat <<END > home/.paludis/environment.conf
world = /dev/null
END

mkdir -p testrepo/{eclass,distfiles,profiles/testprofile,foo/bar/files,foo/baz/files} || exit 1
cd testrepo || exit 1
echo "testrepo" > profiles/repo_name || exit 1
cat <<END > profiles/categories || exit 1
foo
END
cat <<END > profiles/testprofile/make.defaults
ARCH=test
USERLAND=test
KERNEL=test
END
cat <<END > profiles/profiles.desc
test testprofile stable
END

cat <<"END" > foo/bar/bar-1.0.ebuild || exit 1
DESCRIPTION="Test package"
HOMEPAGE="http://paludis.pioto.org/"
SRC_URI=""
SLOT="0"
IUSE=""
LICENSE="GPL-2"
KEYWORDS="test testtest testtesttest"
END

cat <<"END" > foo/bar/bar-2.0.ebuild || exit 1
DESCRIPTION="Test package"
HOMEPAGE="http://paludis.pioto.org/"
SRC_URI=""
SLOT="0"
IUSE=""
LICENSE="GPL-2"
KEYWORDS="~test"
END

cat <<"END" > foo/baz/baz-1.0.ebuild || exit 1
DESCRIPTION="Test package"
HOMEPAGE="http://paludis.pioto.org/"
SRC_URI=""
SLOT="0"
IUSE=""
LICENSE="GPL-2"
KEYWORDS="test testtest testtesttest"
END
cd ..

mkdir -p slaverepo/{eclass,distfiles,profiles/testprofile,foo/bar/files} || exit 1
cd slaverepo || exit 1
echo "slaverepo" > profiles/repo_name || exit 1
cat <<END > profiles/testprofile/make.defaults
ARCH=test
USERLAND=test
KERNEL=test
END
cat <<END > profiles/profiles.desc
test testprofile stable
END


cat <<END > profiles/testprofile/make.defaults
ARCH=test
USERLAND=test
KERNEL=test
END
cat <<END > profiles/profiles.desc
test testprofile stable
END

cd ..

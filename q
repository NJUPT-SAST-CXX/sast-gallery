[33mcommit 281d2615bc7b3f06a12219658f5e573e5db536cd[m[33m ([m[1;32mmain[m[33m)[m
Author: old1ight <1612323700@qq.com>
Date:   Sat Feb 15 23:37:55 2025 +0800

    必做功能基本完成，第1次提交

[33mcommit 6b84b74497a7d271a1a4a8481fef93d5b1f16a1a[m[33m ([m[1;36mHEAD[m[33m, [m[1;31morigin/main[m[33m)[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Sun Feb 9 21:36:43 2025 +0800

    docs: pr template

[33mcommit 37e177003a9516d0546fe5e4981555a1dcc14e8b[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Sun Feb 9 21:26:44 2025 +0800

    docs: pr template and readme

[33mcommit 060f926e527920ef4661b8a9473727481922dfb6[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Sun Feb 9 19:32:15 2025 +0800

    feat(debug): model viewer for showing raw data

[33mcommit 4974abbf3f7b1b478d5de7b3d957dd95b800bc0a[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Sun Feb 9 19:21:00 2025 +0800

    fix(`GalleryWidget`): disconnect old model before connecting a new one
    
    from: #9

[33mcommit 445a46d85b492fa0b2820817821742b0edb17001[m
Author: Mairon <3043462073@qq.com>
Date:   Sat Feb 8 17:38:55 2025 +0800

    feat: tool tip for `MediaViewer`

[33mcommit d6c4d1f725c3986fea1c366880fc958ad9528b64[m
Author: Mairon <3043462073@qq.com>
Date:   Wed Feb 5 14:03:36 2025 +0800

    fix: `ElaMessageBar` display bug of `MediaViewer`

[33mcommit b255c3c6bd3b6dc13f908200f0f76d64c055066e[m
Author: Mairon <3043462073@qq.com>
Date:   Tue Feb 4 21:50:45 2025 +0800

    fix: lifetime of `DiskScanner`
    
    Since `MediaViewer` may exist independently without `MainWindow`, `DiskScanner`, alongside with `MediaListModel`, shall be global single pattern, whose initialization gets called after `QApplication`'s, to ensure `MediaViewer` works fine.
    Given their lifetime, don't bother handling  memory leaks!

[33mcommit 816a1da1e51aa4529231bdea1d0c527ec9be45bd[m
Author: Mairon <3043462073@qq.com>
Date:   Tue Feb 4 20:09:32 2025 +0800

    fix&refactor!: zooming focus gets lost when resizing, refactor `MediaViewerDelegate` & `ImageViewer`

[33mcommit 9d7ef70266775cb188def2eb0488c712b1409efe[m
Author: Mairon <3043462073@qq.com>
Date:   Mon Feb 3 22:24:17 2025 +0800

    fix: zoom bug caused by resizing `ImageViewer`

[33mcommit fd6b27c496343401eec5fc9acfe19cd21b8543a4[m
Author: Mairon <3043462073@qq.com>
Date:   Mon Feb 3 22:07:11 2025 +0800

    refactor!: improve `Full File Info`

[33mcommit 1bd38ff1be915f84a9727c3f1da78a47f59c1145[m
Author: Mairon <3043462073@qq.com>
Date:   Mon Feb 3 15:58:14 2025 +0800

    chore: remove redundant header files

[33mcommit 6cfaa2ede542d883e877047a16a85c019bbe6a52[m
Author: Serein <2075337935@qq.com>
Date:   Mon Feb 3 12:52:16 2025 +0800

    chore: remove redundant `QObject` include

[33mcommit 2c8978b31e7b3f5f216e0a102ce33e77fb61c569[m
Author: Serein <2075337935@qq.com>
Date:   Mon Feb 3 12:49:42 2025 +0800

    fix: lifetime of `DiskScanner` & page margins & non return

[33mcommit 94ed234be76ca4cd27519f4832fb1b4a559639c9[m
Author: wyq777 <635118061@qq.com>
Date:   Sat Feb 1 21:59:11 2025 +0800

    refactor(MediaViewerDelegate): beautify the UI of `Full File Info`

[33mcommit f04cf13fb5f6de54b1d7c6a077afbfbc7f11ad69[m
Author: Mairon <3043462073@qq.com>
Date:   Sat Feb 1 19:20:58 2025 +0800

    fix&refactor!: image zooming and various bugs related to it

[33mcommit a58ddd1e999ee34cc85f36364b6305c4622b0794[m
Author: wyq777 <635118061@qq.com>
Date:   Fri Jan 31 23:44:09 2025 +0800

    refactor(`MediaViewerDelegate`): replace UI of `readFullInfo` with `ElaWidget`

[33mcommit 0df1edde9910d44ba73e7f632027e178d4bc19ae[m
Author: wyq777 <635118061@qq.com>
Date:   Fri Jan 31 22:20:55 2025 +0800

    refactor(`MediaViewer`): improve the algorithm of wheeling to zoom

[33mcommit ac761d1ad72281c7445af3a6490985347aecd999[m
Author: wyq777 <635118061@qq.com>
Date:   Fri Jan 31 22:09:59 2025 +0800

    refactor: move `ImageViewer::wheelEvent` to `MediaViewer`

[33mcommit 7f315780e0e8be3069821eabd9a750eda9c04c9f[m[33m ([m[1;32mlocal_change[m[33m, [m[1;32mbranch[m[33m)[m
Author: Mairon <3043462073@qq.com>
Date:   Fri Jan 31 12:15:20 2025 +0800

    fix(`ImageViewer`): prevent zooming when disabled

[33mcommit 9654fc889c782e98e6870803e91a110aad02fa17[m
Author: wyq777 <635118061@qq.com>
Date:   Thu Jan 30 23:55:37 2025 +0800

    refactor: improve the experience of zooming

[33mcommit 3de33f89b70eda9fabc2ee0f23c87ba59a6ee8a9[m
Author: Mairon <3043462073@qq.com>
Date:   Thu Jan 30 23:20:58 2025 +0800

    refactor!: move components from `utils` to `view/component`

[33mcommit 629cd21132c4ec9bf8209eccc95762e7f17e83a5[m
Author: wyq777 <635118061@qq.com>
Date:   Thu Jan 30 15:46:34 2025 +0800

    fix: can't change value of zoomSlider when wheeling to zoom

[33mcommit e124497d4eb0d3e6c2cf2a7bf48336842acf8557[m
Author: Mairon <3043462073@qq.com>
Date:   Thu Jan 30 15:56:02 2025 +0800

    fix:(`MediaViewer`): fail to resize through mouse on Linux

[33mcommit e63e82bd5a91fa558e2a8bce5181d616eb3b3fc9[m
Author: AstroAir <astro_air@126.com>
Date:   Thu Jan 30 13:40:56 2025 +0800

    refactor: replace QDirListing with QDirIterator for directory scanning

[33mcommit 387f1308026edd293399202eb6f4a2999c48788c[m
Author: Mairon <3043462073@qq.com>
Date:   Thu Jan 30 11:40:24 2025 +0800

    feat(`MediaViewerDelegate`): add wheel event handling for zooming and navigation

[33mcommit 1873916ecd51849bd6da208449e4119f3dcab63a[m
Author: wyq777 <635118061@qq.com>
Date:   Thu Jan 30 11:14:02 2025 +0800

    fix: can't zoom by dragging zoomSlider

[33mcommit 469e84ae3af105f0a11d48f7dafc1b595f2899e0[m
Author: Mairon <3043462073@qq.com>
Date:   Thu Jan 30 03:04:07 2025 +0800

    fix(`MediaViewerDelegate`): various bugs emitted by image load error

[33mcommit 6abef5aa4dae2a4e111b33ef0d01044e4c166a53[m
Author: Mairon <3043462073@qq.com>
Date:   Thu Jan 30 02:20:37 2025 +0800

    feat: `ImageViewer` for display image with zooming and dragging supported

[33mcommit 3690149ee4bbcab871c7072090f600fb231c43cd[m
Author: wyq777 <635118061@qq.com>
Date:   Tue Jan 28 16:32:46 2025 +0800

    feat&fix(`MediaViewerDelegate`): rotateImage & fix problem that can't zoom in

[33mcommit b0cc1c71d5ad8cccc05c6bbe5a77c897c99902a2[m
Author: wyq777 <635118061@qq.com>
Date:   Tue Jan 28 16:23:52 2025 +0800

    feat(`MediaViewerDelegate`): scaleImage
    
    ONLY SUPPORT BUTTON RESIZE NOW
    MOUSE WHEEL RESIZE TO BE SUPPORTED

[33mcommit 7f0e1a418e065c5aa8df7dd5b7762c35a5e273a4[m
Author: wyq777 <635118061@qq.com>
Date:   Tue Jan 28 16:13:14 2025 +0800

    fix: can't properly open Image from File Explorer

[33mcommit 641b9f5d08a0c85285c40b1b7ac3d68f90cf54a7[m
Author: wyq777 <635118061@qq.com>
Date:   Tue Jan 28 14:21:31 2025 +0800

    chore&perf: exception process of open image and notes of the project improved

[33mcommit 851c72fa0eeee8aea8c011f4129ff5fc50935387[m
Author: Mairon <3043462073@qq.com>
Date:   Tue Jan 28 14:11:18 2025 +0800

    docs: add pre-commit introduction

[33mcommit d2d8f46d7469b9612800adcc0afefa2b15278e3d[m
Author: wyq777 <635118061@qq.com>
Date:   Tue Jan 28 13:34:15 2025 +0800

    feat(`MediaViewer`): zoomSlider activated

[33mcommit d5f0df1e11d1cc389ebb777e542d34321802237d[m
Author: wyq777 <635118061@qq.com>
Date:   Tue Jan 28 12:53:44 2025 +0800

    fix(`MediaViewerDelegate`): can't read memory when zooming

[33mcommit 597b850451647197522c71c727ce13b31642c003[m
Author: Mairon <3043462073@qq.com>
Date:   Tue Jan 28 11:57:22 2025 +0800

    fix: logic error of image deletion

[33mcommit 55dcbcaa9eeb714654dbbd02263e72c1ca4b98c2[m
Author: Mairon <3043462073@qq.com>
Date:   Tue Jan 28 11:47:34 2025 +0800

    refactor(`MediaViewerDelegate`): simplify logic by `QPersistentModelIndex`

[33mcommit 01a8f3868d6afeef64e1fa56378d47948d19580f[m
Author: Mairon <3043462073@qq.com>
Date:   Tue Jan 28 09:24:41 2025 +0800

    feat: model row insertion and removal handling of `MediaViewerDelegate`

[33mcommit de0d3509c6c836b1d41f40478cb80f3331975921[m
Author: Mairon <3043462073@qq.com>
Date:   Tue Jan 28 04:22:50 2025 +0800

    BREAKING CHANGE: reafctor `ViewingWindow` and `ImageViewer` into `MediaViewer`

[33mcommit 776ea6fcac6fe17f413f3b690fedaaf9f7eeb214[m
Author: wyq777 <635118061@qq.com>
Date:   Tue Jan 28 00:03:33 2025 +0800

    refactor(`ViewingWindwo`&`ImageViewer`): structure improved

[33mcommit 1502d14e09f160bdad482ff1fba6a272a33d8901[m
Author: wyq777 <635118061@qq.com>
Date:   Mon Jan 27 23:52:43 2025 +0800

    fix&refactor(`ViewingWindow`): readFullInfo no longer open File Explorer when called

[33mcommit 2e56eaeeba27fe37c4a07430f477c66bc0e063f4[m
Author: wyq777 <635118061@qq.com>
Date:   Mon Jan 27 21:37:56 2025 +0800

    fix(`ViewingWindow`): delete redundant layout

[33mcommit e361504e73ac041f197d7c63e87f7c150b87998b[m
Author: wyq777 <635118061@qq.com>
Date:   Mon Jan 27 16:31:26 2025 +0800

    fix(`ViewingWindow`): change fullscreen button to maximize window

[33mcommit 892baca57acc634896b3725bea02aebaf2be06bc[m
Author: wyq777 <635118061@qq.com>
Date:   Mon Jan 27 16:25:51 2025 +0800

    fix: fix the layout of `ViewingWindow`

[33mcommit ef707923c91e05fad75d0d0b08cf1ef5c430b73b[m
Author: Mairon <3043462073@qq.com>
Date:   Mon Jan 27 14:55:10 2025 +0800

    feat&refactor(`MediaViewer`): remove highlight effect, add scale effect for click

[33mcommit 991bf303c4a584396807292cc8d0f0f19a10c560[m
Author: wyq777 <635118061@qq.com>
Date:   Mon Jan 27 12:13:47 2025 +0800

    feat&refactor(`ImageViewer`):  adaptiveResize for sizing window
    default: resize window to 1/4 area of the monitor

[33mcommit 662b680576669e8ae5d1220acbc0a21d6c962441[m
Author: wyq777 <635118061@qq.com>
Date:   Mon Jan 27 11:21:13 2025 +0800

    feat(`ImageViewer`): add method to retrieve image and adjust window size based on image resolution

[33mcommit 87165f357f55bbc06037d865ea1ea431a398b605[m
Author: Mairon <3043462073@qq.com>
Date:   Sun Jan 26 23:47:36 2025 +0800

    feat: add `utils/Tools` for common functions

[33mcommit aca96b97d9f0febf320266667de1c7af6b8cb996[m
Author: Mairon <3043462073@qq.com>
Date:   Sun Jan 26 22:44:15 2025 +0800

    feat: highlight effect on hover of `MediaPreviewer`

[33mcommit 4af39726e617394d0c350ecdaa9967b05f0d5385[m
Author: wyq777 <635118061@qq.com>
Date:   Sun Jan 26 21:53:11 2025 +0800

    chore: update notes of zoom in `ViewingWindow`

[33mcommit b2b3fb5c5b6e201c58736bfb1369de21b3f5ff91[m
Author: wyq777 <635118061@qq.com>
Date:   Sun Jan 26 21:14:36 2025 +0800

    chore: notes of zoom in/out button&slider

[33mcommit 039e19f8ac3d1ad9a45f5391dddf73f08381de8a[m
Author: wyq777 <635118061@qq.com>
Date:   Sun Jan 26 21:11:41 2025 +0800

    feat(`ViewingWindow`):showFullScreen

[33mcommit b889eccba06ed8a8b68f34dfac0e9a5139c4d80c[m
Author: Mairon <3043462073@qq.com>
Date:   Sun Jan 26 20:47:23 2025 +0800

    fix(ci): linux package errors

[33mcommit 5b85014ac83d9689025a00d4c70cb226699758e0[m
Author: Mairon <3043462073@qq.com>
Date:   Sun Jan 26 20:27:22 2025 +0800

    chore(deps): revert submodule for build & deploy

[33mcommit 9fe6ec09b83e9af7a7ffefe47f16a043a969294e[m
Author: Mairon <3043462073@qq.com>
Date:   Sun Jan 26 18:27:22 2025 +0800

    refactor!: `MediaPreviewer` and `ViewingWindow`  need model and row index

[33mcommit 373130c609d75ab90dbd2aa2134b7cdb3f752402[m
Author: wyq777 <635118061@qq.com>
Date:   Sun Jan 26 17:32:21 2025 +0800

    feat(`ImageViewer`): show Full ImageInfo
    
    NOW JUST FOR DEBUGGING

[33mcommit 5ff2f929303bf124b3af57206edc3a2b7023bfe0[m
Author: dependabot[bot] <49699333+dependabot[bot]@users.noreply.github.com>
Date:   Sun Jan 26 09:25:20 2025 +0000

    chore(deps): bump 3rdpart/ElaWidgetTools from `47dc3f7` to `5783f2e`
    
    Bumps [3rdpart/ElaWidgetTools](https://github.com/Liniyous/ElaWidgetTools) from `47dc3f7` to `5783f2e`.
    - [Commits](https://github.com/Liniyous/ElaWidgetTools/compare/47dc3f7f2531ddbe1d103ffc1ae6fd3a863f17a0...5783f2e76b8493a75578e2120387a7816719f1b8)
    
    ---
    updated-dependencies:
    - dependency-name: 3rdpart/ElaWidgetTools
      dependency-type: direct:production
    ...
    
    Signed-off-by: dependabot[bot] <support@github.com>

[33mcommit 537636c38bfe76e1b337b69ae2f5e8b8ca9d23f3[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Sun Jan 26 17:04:48 2025 +0800

    perf: improve layout performance

[33mcommit 3775b13e62de4ca1d1ad2c463e53de50cb2dcff3[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Sun Jan 26 15:07:19 2025 +0800

    fix: `MediaPreviewer::roundedPixmap` radius not work

[33mcommit 01ab410de060ea19ad64e7bd060e78b555ff864f[m
Author: Mairon <3043462073@qq.com>
Date:   Sun Jan 26 13:02:15 2025 +0800

    feat: rounded corner of `MediaPreviewer`

[33mcommit ab655d485edad3b03ed807dbccb926b80e70e826[m
Author: wyq777 <635118061@qq.com>
Date:   Sun Jan 26 11:10:12 2025 +0800

    chore: TODOs in `ViewingWindow` for Juniors

[33mcommit 8a7f0ba1a9b02cac32885cefae204ff14491e73b[m
Author: wyq777 <635118061@qq.com>
Date:   Sun Jan 26 10:41:28 2025 +0800

    feat(`ImageViewer`): save image as file

[33mcommit 3760c59cbc97bcb8dc498ca8838f534fbb1bab10[m
Author: wyq777 <635118061@qq.com>
Date:   Sun Jan 26 10:30:38 2025 +0800

    feat(`ImageViewer`): copy image to Clipboard

[33mcommit 399d104146bac871ce1a024d2daf7fb940c50925[m
Author: wyq777 <635118061@qq.com>
Date:   Sun Jan 26 09:49:54 2025 +0800

    fix(UI): ban Image moving after loading in `ViewingWindow`

[33mcommit 09e2734b9b81653a5a0941eed599a60e82b6f8b9[m
Author: wyq777 <635118061@qq.com>
Date:   Sun Jan 26 09:33:49 2025 +0800

    fix: fix logical problem of UI in `ViewingWindow`

[33mcommit 6c47ecfb43298da38bfa8c88834e1d7687869c17[m
Author: wyq777 <635118061@qq.com>
Date:   Sat Jan 25 23:42:28 2025 +0800

    style: formatted

[33mcommit 52411e56a3e0fa677e2958fcfd74f15b268b3c3c[m
Author: wyq777 <635118061@qq.com>
Date:   Sat Jan 25 23:31:03 2025 +0800

    feat: loadImage in `ViewingWindow` through `ImageViewer`

[33mcommit 334b3c061e256a7e21180677c8cd049c7f9a4faa[m
Author: Mairon <3043462073@qq.com>
Date:   Sat Jan 25 23:34:40 2025 +0800

    refactor: simplify area creation of `SettingPage` and `AboutPage`

[33mcommit 554232e5d02dc65dbf409e7a5e2a385abc23aaa4[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Sat Jan 25 23:10:00 2025 +0800

    fix: disk scanner won't scan video

[33mcommit 4059e88b85b384508943bda9b31ec77b3c5ddad8[m
Author: Mairon <3043462073@qq.com>
Date:   Sat Jan 25 22:05:11 2025 +0800

    feat&refactor: `ViewingWindow` accept a filepath to construct

[33mcommit 3bdf79e81965b069c8ff2223ee6edf38b5c08d94[m
Author: wyq777 <635118061@qq.com>
Date:   Sat Jan 25 16:23:17 2025 +0800

    feat: provide connect&signals without actual slots in `ViewingWindow`

[33mcommit 44c1da8fee219102ee5880647e2d6fc7bde8963c[m
Author: Mairon <3043462073@qq.com>
Date:   Sat Jan 25 15:24:57 2025 +0800

    feat&refactor: improve ui of `ViewingWindow`

[33mcommit 857dede01db224ce3209eaf06c55c517f063fb84[m
Author: Mairon <3043462073@qq.com>
Date:   Sat Jan 25 11:47:59 2025 +0800

    fix(build): `ElaWidgetTools` build failure

[33mcommit 9e945ad9d70c703570e5a58c43e75869e3cbe8c6[m
Author: wyq777 <635118061@qq.com>
Date:   Sat Jan 25 00:06:14 2025 +0800

    fix: include ElaIconButton.h

[33mcommit b4f0fb11b7dd5af273c33fdeb17a4ea4b166aa5e[m
Author: Mairon <3043462073@qq.com>
Date:   Fri Jan 24 23:52:22 2025 +0800

    refactor: remove unnessary layout in `AboutPage`

[33mcommit ec7f997b953aea862cb038af6e3c2092088d2cad[m
Author: wyq777 <635118061@qq.com>
Date:   Fri Jan 24 23:46:36 2025 +0800

    refactor: ElaPushButton to ElaIconButton

[33mcommit d3414d0ad275fed445185875de8fcb652d1e3ef4[m
Author: dependabot[bot] <49699333+dependabot[bot]@users.noreply.github.com>
Date:   Fri Jan 24 23:25:39 2025 +0800

    chore(deps): bump 3rdpart/ElaWidgetTools from `5e1a438` to `47dc3f7` (#3)
    
    Bumps [3rdpart/ElaWidgetTools](https://github.com/Liniyous/ElaWidgetTools) from `5e1a438` to `47dc3f7`.
    - [Commits](https://github.com/Liniyous/ElaWidgetTools/compare/5e1a438b821b2ef896252c77a913e8c0e8814324...47dc3f7f2531ddbe1d103ffc1ae6fd3a863f17a0)
    
    ---
    updated-dependencies:
    - dependency-name: 3rdpart/ElaWidgetTools
      dependency-type: direct:production
    ...
    
    Signed-off-by: dependabot[bot] <support@github.com>
    Co-authored-by: dependabot[bot] <49699333+dependabot[bot]@users.noreply.github.com>

[33mcommit dd775a853ad392a79eb10b1c8c17b26550f24736[m
Author: Mairon <3043462073@qq.com>
Date:   Fri Jan 24 23:08:15 2025 +0800

    feat: improve `AboutPage`

[33mcommit 2c70ee31635e4c02b98cfc5a65c75c13f5b331eb[m
Author: wyq777 <635118061@qq.com>
Date:   Fri Jan 24 22:54:17 2025 +0800

    style: formatted

[33mcommit 504281d750b59f2e39f7fb30feacf96df122219b[m
Author: wyq777 <635118061@qq.com>
Date:   Fri Jan 24 22:51:02 2025 +0800

    feat: ImageViewer now supports high-DPI display

[33mcommit f00f079e268aacbba8845c4179a901fd0d75d947[m
Author: wyq777 <635118061@qq.com>
Date:   Fri Jan 24 22:38:27 2025 +0800

    style: clang-formatted

[33mcommit c33c76c2c63dabc11aa7e8e44270966c03be68bb[m
Author: wyq777 <635118061@qq.com>
Date:   Fri Jan 24 22:36:27 2025 +0800

    refactor&feat!: MediaViewer --> ImageViewer
    
    remove functions for video playing

[33mcommit 0dd3a0287b7b9592f44b1d1725d4c3e105b08924[m
Author: wyq777 <635118061@qq.com>
Date:   Fri Jan 24 22:16:15 2025 +0800

    feat&refactor: viewingWindow UI tiny demo

[33mcommit 54dd07a0bc21d6953361d349f7cf8165eefa6fad[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Fri Jan 24 21:31:47 2025 +0800

    build: remove 3rd qrc

[33mcommit c9233591da6c909c96fa65e5e97eb07f01c34990[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Fri Jan 24 21:09:45 2025 +0800

    feat: implement `GalleryWidget`, connect signals

[33mcommit df6fb36663b915aaea55df5eba42327e3b73ddc8[m
Author: szxc <shuzhuxvchuang@163.com>
Date:   Fri Jan 24 20:44:32 2025 +0800

    feat: implement AboutPage

[33mcommit 7b15c083eac9fc5af4a4ada632cd4ad8c794c764[m
Author: Mairon <3043462073@qq.com>
Date:   Fri Jan 24 18:13:53 2025 +0800

    feat: add windows manifest file for application compatibility

[33mcommit 6082ca3a2bc5af6fd8d8bcb7daeac368ee0071dd[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Fri Jan 24 11:56:48 2025 +0800

    chore: gitignore and pre-commit

[33mcommit 4f09a0215b00d77f328da50d1570ad2d7d8003e2[m
Author: wyq777 <635118061@qq.com>
Date:   Fri Jan 24 11:23:42 2025 +0800

    style: clang-formatted

[33mcommit fbe987d62fd29d9db390d2763915c93c0e806326[m
Author: Mairon <3043462073@qq.com>
Date:   Fri Jan 24 01:03:52 2025 +0800

    fix: settings initialization

[33mcommit b52f91fab8770fbe0a5379a4e6e28bc636a0efbf[m
Author: Mairon <3043462073@qq.com>
Date:   Fri Jan 24 00:42:36 2025 +0800

    fix(package): windows installer output dir

[33mcommit b052ce73b8507590ed2460f1e505aa9cd9ca1763[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Thu Jan 23 23:42:50 2025 +0800

    refactor: update `MediaListModel`

[33mcommit 8af72661d92032f87d15f9ef68c856e9fb9f35dd[m
Merge: de383e2 055c7bb
Author: wyq777 <635118061@qq.com>
Date:   Thu Jan 23 23:28:09 2025 +0800

    Merge branch 'main' of https://github.com/NJUPT-SAST-CXX/sast-gallery

[33mcommit de383e26452e6a0f05e3d77cae9ff3d55cc97cb1[m
Author: wyq777 <635118061@qq.com>
Date:   Thu Jan 23 23:27:53 2025 +0800

    feat&refactor: add  ViewingWindow with menuBar

[33mcommit 055c7bbf867e7a467c10fa4bcd022cba13c97622[m
Author: Mairon <3043462073@qq.com>
Date:   Thu Jan 23 22:57:33 2025 +0800

    feat: version support

[33mcommit 387e5a82f27d8209e050def34eb6841bc5292e17[m
Author: wyq777 <635118061@qq.com>
Date:   Thu Jan 23 22:14:55 2025 +0800

    refactor: rename Pagenode from Viewing to ViewingMedia

[33mcommit 0c325d53803221af46ab88cc7282547b63468e07[m
Author: wyq777 <635118061@qq.com>
Date:   Thu Jan 23 22:08:38 2025 +0800

    feat: add PageNode for showing ViewingWindow

[33mcommit 557cf62babad7f7fad4e694aca2cbd46df954a58[m
Author: Mairon <3043462073@qq.com>
Date:   Thu Jan 23 16:23:02 2025 +0800

    fix(ci): clang-tidy-review

[33mcommit 278936eac1ddf3331d552bc725ce3e0abb19fb2b[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Thu Jan 23 16:09:00 2025 +0800

    style: silent format check

[33mcommit 5392716a3007d5a25dbd34a34faf99b1a874ebfa[m
Author: Mairon <3043462073@qq.com>
Date:   Thu Jan 23 16:01:40 2025 +0800

    fix(ci): clang-tidy-review

[33mcommit b97946a8c45b13a76ec22dc44de84624d66d695f[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Thu Jan 23 15:58:36 2025 +0800

    feat: implement `DiskScanner`

[33mcommit 46114fff8dd8af92ddeed15902dc1cd7d37060f3[m
Author: Mairon <3043462073@qq.com>
Date:   Thu Jan 23 15:44:19 2025 +0800

    feat(build): macOS build and deploy

[33mcommit bbc2ae62414fd0dbb6879dd0ccf5e365fcc8b90f[m
Author: Mairon <3043462073@qq.com>
Date:   Thu Jan 23 12:37:59 2025 +0800

    feat(build): use static library of `ElaWidgetTools`, remove Qt5 support

[33mcommit 88c9939f7570c3d10b38cd7927f5010b21c94ac1[m
Author: Mairon <3043462073@qq.com>
Date:   Thu Jan 23 02:02:30 2025 +0800

    feat(ci): windows/linux build and deploy

[33mcommit 617cf5bc04fd37b61432980ac7c66e12b0766437[m
Author: Mairon <3043462073@qq.com>
Date:   Thu Jan 23 01:59:05 2025 +0800

    fix: `app.rc` not included

[33mcommit 9dc7db98a7d5bba500a7c9b237a9801e415d3935[m
Author: Mairon <3043462073@qq.com>
Date:   Wed Jan 22 23:46:58 2025 +0800

    feat: add application icon

[33mcommit 074e55a65560d31033f44af7d8c204a97cf6ee19[m
Author: Mairon <3043462073@qq.com>
Date:   Wed Jan 22 23:36:30 2025 +0800

    feat(doc): add README

[33mcommit 777713de36c93ad483a5acd41a455c3f93aa613c[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Wed Jan 22 18:14:13 2025 +0800

    feat!: implement list model, rename class

[33mcommit 9e2cfe24753397353003781e9c74113600843714[m
Author: Mairon <3043462073@qq.com>
Date:   Wed Jan 22 01:07:54 2025 +0800

    feat: implement `SettingPage`

[33mcommit cb9f43e8669580f8be1f81ebfce078d6d50bc194[m
Author: Mairon <3043462073@qq.com>
Date:   Tue Jan 21 23:45:44 2025 +0800

    feat: add `settings`

[33mcommit e0ab1274883a4284fb4212903541b108af078a7b[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Tue Jan 21 23:37:12 2025 +0800

    feat: implement `MediaFlexLayout`

[33mcommit c1cd5ce998b87f56e606efa891d117257e0045ae[m
Author: Mairon <3043462073@qq.com>
Date:   Tue Jan 21 22:37:05 2025 +0800

    feat: add `ViewingWindow`

[33mcommit 717f1bdaf704e80244ecdc4db653ed0fa1376b35[m
Author: Mairon <3043462073@qq.com>
Date:   Tue Jan 21 22:20:55 2025 +0800

    feat: enable DeveloperComponents of ElaWidgetTools

[33mcommit 450ad70692ef8a5f48981821cccbf0bc5b36efeb[m
Author: wyq777 <635118061@qq.com>
Date:   Tue Jan 21 21:19:20 2025 +0800

    fix(MediaViewer): noted the virtual interfaces for further modifying

[33mcommit 31930a472e215f50bee9bf4c4d4da1cefa8a9bd0[m
Author: wyq777 <635118061@qq.com>
Date:   Tue Jan 21 20:15:59 2025 +0800

    feat(MediaViewer): virtual interfaces for media viewing

[33mcommit ef05d9e4f75c0a1df73fdbab4858c5715fa39750[m
Author: wyq777 <635118061@qq.com>
Date:   Tue Jan 21 17:01:50 2025 +0800

    feat(MediaViewer): loadImage,loadVideo

[33mcommit fe89c2b3efd7f5a7d98c6a20dd2a1543b70c0dd0[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Tue Jan 21 11:53:59 2025 +0800

    ci: add qt multimedia and widgets
    
    `ZedThree/clang-tidy-review@v0.19.0` only supposed to be used in PR

[33mcommit b69a18ba4450694936f66eda5cfb78d98d772d6a[m
Author: wyq777 <635118061@qq.com>
Date:   Tue Jan 21 11:10:54 2025 +0800

    refacor&feat!: ImageViewer  --> MediaViewer   & add some functional infrastructure

[33mcommit ba55ab59b6c75c888fd5486d91bcf0ef6cc8821a[m
Author: Mairon <3043462073@qq.com>
Date:   Mon Jan 20 01:00:02 2025 +0800

    fix(ci): qt version for install

[33mcommit 866e382afa4c8a8fdc510380ee786ce7b8ed5804[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Sun Jan 19 23:12:53 2025 +0800

    feat: correct page name, add placeholder

[33mcommit 1adfab55254ce8db4872b2ba9e8eb352e78b041a[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Sun Jan 19 15:26:01 2025 +0800

    feat: added basic `ImageViewer`

[33mcommit 0742ebf90eaa724b15a4ab161f8d52a2a3878665[m
Author: cEvolve05 <cEvolve05@gmail.com>
Date:   Sat Jan 18 21:38:18 2025 +0800

    feat: basic view and structure
    
    - added `BasePage` and other pages in src/view
    - remove redundant code

[33mcommit 7ef780dd7c09fa7c28078f5ab78362f916b103f5[m
Author: Mairon <3043462073@qq.com>
Date:   Wed Jan 15 21:46:54 2025 +0800

    initial commit

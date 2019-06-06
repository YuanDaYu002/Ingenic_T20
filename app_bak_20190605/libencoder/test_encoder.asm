
test:     file format elf32-tradlittlemips


Disassembly of section .text:

00401050 <main>:
  401050:	27bdffe0 	addiu	sp,sp,-32
  401054:	afbf001c 	sw	ra,28(sp)
  401058:	0c100b6b 	jal	402dac <encoder_system_init>
  40105c:	00000000 	nop
  401060:	0c100601 	jal	401804 <timestamp_update_task>
  401064:	00000000 	nop
  401068:	0c1006e8 	jal	401ba0 <video_get_h264_stream_task>
  40106c:	00000000 	nop
  401070:	0c1009bb 	jal	4026ec <jpeg_get_one_snap>
  401074:	00002021 	move	a0,zero
  401078:	0c1009bb 	jal	4026ec <jpeg_get_one_snap>
  40107c:	24040001 	li	a0,1
  401080:	0c100ee8 	jal	403ba0 <sleep@plt>
  401084:	24040002 	li	a0,2
  401088:	0c100be6 	jal	402f98 <encoder_system_exit>
  40108c:	00000000 	nop
  401090:	8fbf001c 	lw	ra,28(sp)
  401094:	00001021 	move	v0,zero
  401098:	03e00008 	jr	ra
  40109c:	27bd0020 	addiu	sp,sp,32

004010a0 <timestamp_update_func>:
  4010a0:	27bdff68 	addiu	sp,sp,-152
  4010a4:	afbf0094 	sw	ra,148(sp)
  4010a8:	afbe0090 	sw	s8,144(sp)
  4010ac:	afb7008c 	sw	s7,140(sp)
  4010b0:	afb60088 	sw	s6,136(sp)
  4010b4:	afb50084 	sw	s5,132(sp)
  4010b8:	afb40080 	sw	s4,128(sp)
  4010bc:	afb3007c 	sw	s3,124(sp)
  4010c0:	afb20078 	sw	s2,120(sp)
  4010c4:	afb10074 	sw	s1,116(sp)
  4010c8:	0c100edc 	jal	403b70 <pthread_self@plt>
  4010cc:	afb00070 	sw	s0,112(sp)
  4010d0:	0c100f90 	jal	403e40 <pthread_detach@plt>
  4010d4:	00402021 	move	a0,v0
  4010d8:	3404aa00 	li	a0,0xaa00
  4010dc:	afa00048 	sw	zero,72(sp)
  4010e0:	0c100f18 	jal	403c60 <malloc@plt>
  4010e4:	afa0004c 	sw	zero,76(sp)
  4010e8:	3404aa00 	li	a0,0xaa00
  4010ec:	afa20068 	sw	v0,104(sp)
  4010f0:	0c100f18 	jal	403c60 <malloc@plt>
  4010f4:	afa20048 	sw	v0,72(sp)
  4010f8:	8fa30068 	lw	v1,104(sp)
  4010fc:	afa2006c 	sw	v0,108(sp)
  401100:	106000c2 	beqz	v1,40140c <timestamp_update_func+0x36c>
  401104:	afa2004c 	sw	v0,76(sp)
  401108:	104000ae 	beqz	v0,4013c4 <timestamp_update_func+0x324>
  40110c:	3c020042 	lui	v0,0x42
  401110:	3c100041 	lui	s0,0x41
  401114:	8c44c3c8 	lw	a0,-15416(v0)
  401118:	8e054100 	lw	a1,16640(s0)
  40111c:	0c100f64 	jal	403d90 <IMP_OSD_ShowRgn@plt>
  401120:	24060001 	li	a2,1
  401124:	144000a5 	bnez	v0,4013bc <timestamp_update_func+0x31c>
  401128:	3c030042 	lui	v1,0x42
  40112c:	2463c3c8 	addiu	v1,v1,-15416
  401130:	26104100 	addiu	s0,s0,16640
  401134:	8c640004 	lw	a0,4(v1)
  401138:	8e050004 	lw	a1,4(s0)
  40113c:	0c100f64 	jal	403d90 <IMP_OSD_ShowRgn@plt>
  401140:	24060001 	li	a2,1
  401144:	14400080 	bnez	v0,401348 <timestamp_update_func+0x2a8>
  401148:	27a3002c 	addiu	v1,sp,44
  40114c:	afa0005c 	sw	zero,92(sp)
  401150:	27be0050 	addiu	s8,sp,80
  401154:	afa30058 	sw	v1,88(sp)
  401158:	3411aa00 	li	s1,0xaa00
  40115c:	0c100ee0 	jal	403b80 <time@plt>
  401160:	03c02021 	move	a0,s8
  401164:	0c100f34 	jal	403cd0 <localtime@plt>
  401168:	03c02021 	move	a0,s8
  40116c:	3c060040 	lui	a2,0x40
  401170:	27a40018 	addiu	a0,sp,24
  401174:	00403821 	move	a3,v0
  401178:	24050028 	li	a1,40
  40117c:	24c63154 	addiu	a2,a2,12628
  401180:	00809821 	move	s3,a0
  401184:	afa00064 	sw	zero,100(sp)
  401188:	afa00060 	sw	zero,96(sp)
  40118c:	afa00018 	sw	zero,24(sp)
  401190:	afa0001c 	sw	zero,28(sp)
  401194:	afa00020 	sw	zero,32(sp)
  401198:	afa00024 	sw	zero,36(sp)
  40119c:	afa00028 	sw	zero,40(sp)
  4011a0:	afa0002c 	sw	zero,44(sp)
  4011a4:	afa00030 	sw	zero,48(sp)
  4011a8:	afa00034 	sw	zero,52(sp)
  4011ac:	afa00038 	sw	zero,56(sp)
  4011b0:	0c100f30 	jal	403cc0 <strftime@plt>
  4011b4:	afa0003c 	sw	zero,60(sp)
  4011b8:	82630000 	lb	v1,0(s3)
  4011bc:	2462ffe0 	addiu	v0,v1,-32
  4011c0:	304200ff 	andi	v0,v0,0xff
  4011c4:	2c44001b 	sltiu	a0,v0,27
  4011c8:	10800013 	beqz	a0,401218 <timestamp_update_func+0x178>
  4011cc:	3c040040 	lui	a0,0x40
  4011d0:	00021080 	sll	v0,v0,0x2
  4011d4:	248432e0 	addiu	a0,a0,13024
  4011d8:	00821021 	addu	v0,a0,v0
  4011dc:	8c420000 	lw	v0,0(v0)
  4011e0:	00400008 	jr	v0
  4011e4:	00000000 	nop
  4011e8:	2462ffd0 	addiu	v0,v1,-48
  4011ec:	3c030041 	lui	v1,0x41
  4011f0:	24634170 	addiu	v1,v1,16752
  4011f4:	000210c0 	sll	v0,v0,0x3
  4011f8:	00621021 	addu	v0,v1,v0
  4011fc:	8c430000 	lw	v1,0(v0)
  401200:	8fa40060 	lw	a0,96(sp)
  401204:	8c420004 	lw	v0,4(v0)
  401208:	afa30064 	sw	v1,100(sp)
  40120c:	00832021 	addu	a0,a0,v1
  401210:	afa2005c 	sw	v0,92(sp)
  401214:	afa40060 	sw	a0,96(sp)
  401218:	8fa30064 	lw	v1,100(sp)
  40121c:	8fb00068 	lw	s0,104(sp)
  401220:	27b2004c 	addiu	s2,sp,76
  401224:	0003b080 	sll	s6,v1,0x2
  401228:	8fa30060 	lw	v1,96(sp)
  40122c:	0003a080 	sll	s4,v1,0x2
  401230:	8fb5005c 	lw	s5,92(sp)
  401234:	02148021 	addu	s0,s0,s4
  401238:	0000b821 	move	s7,zero
  40123c:	02f02021 	addu	a0,s7,s0
  401240:	02a02821 	move	a1,s5
  401244:	02c03021 	move	a2,s6
  401248:	0c100ed8 	jal	403b60 <memcpy@plt>
  40124c:	26f70500 	addiu	s7,s7,1280
  401250:	16f1fffa 	bne	s7,s1,40123c <timestamp_update_func+0x19c>
  401254:	02b6a821 	addu	s5,s5,s6
  401258:	13d20028 	beq	s8,s2,4012fc <timestamp_update_func+0x25c>
  40125c:	8fa30058 	lw	v1,88(sp)
  401260:	8e500000 	lw	s0,0(s2)
  401264:	1000fff2 	b	401230 <timestamp_update_func+0x190>
  401268:	26520004 	addiu	s2,s2,4
  40126c:	3c040041 	lui	a0,0x41
  401270:	24844170 	addiu	a0,a0,16752
  401274:	3c020041 	lui	v0,0x41
  401278:	8c840060 	lw	a0,96(a0)
  40127c:	8fa30060 	lw	v1,96(sp)
  401280:	24424170 	addiu	v0,v0,16752
  401284:	8c420064 	lw	v0,100(v0)
  401288:	00641821 	addu	v1,v1,a0
  40128c:	afa40064 	sw	a0,100(sp)
  401290:	afa2005c 	sw	v0,92(sp)
  401294:	1000ffe0 	b	401218 <timestamp_update_func+0x178>
  401298:	afa30060 	sw	v1,96(sp)
  40129c:	3c030041 	lui	v1,0x41
  4012a0:	24634170 	addiu	v1,v1,16752
  4012a4:	8c630054 	lw	v1,84(v1)
  4012a8:	3c020041 	lui	v0,0x41
  4012ac:	24424170 	addiu	v0,v0,16752
  4012b0:	8c420050 	lw	v0,80(v0)
  4012b4:	afa3005c 	sw	v1,92(sp)
  4012b8:	8fa30060 	lw	v1,96(sp)
  4012bc:	afa20064 	sw	v0,100(sp)
  4012c0:	00621821 	addu	v1,v1,v0
  4012c4:	1000ffd4 	b	401218 <timestamp_update_func+0x178>
  4012c8:	afa30060 	sw	v1,96(sp)
  4012cc:	3c040041 	lui	a0,0x41
  4012d0:	24844170 	addiu	a0,a0,16752
  4012d4:	3c020041 	lui	v0,0x41
  4012d8:	8c840058 	lw	a0,88(a0)
  4012dc:	8fa30060 	lw	v1,96(sp)
  4012e0:	24424170 	addiu	v0,v0,16752
  4012e4:	8c42005c 	lw	v0,92(v0)
  4012e8:	00641821 	addu	v1,v1,a0
  4012ec:	afa40064 	sw	a0,100(sp)
  4012f0:	afa2005c 	sw	v0,92(sp)
  4012f4:	1000ffc8 	b	401218 <timestamp_update_func+0x178>
  4012f8:	afa30060 	sw	v1,96(sp)
  4012fc:	26730001 	addiu	s3,s3,1
  401300:	1473ffad 	bne	v1,s3,4011b8 <timestamp_update_func+0x118>
  401304:	3c020042 	lui	v0,0x42
  401308:	8fa30068 	lw	v1,104(sp)
  40130c:	8c44c3c8 	lw	a0,-15416(v0)
  401310:	27a50040 	addiu	a1,sp,64
  401314:	0c100f94 	jal	403e50 <IMP_OSD_UpdateRgnAttrData@plt>
  401318:	afa30040 	sw	v1,64(sp)
  40131c:	3c020042 	lui	v0,0x42
  401320:	2442c3c8 	addiu	v0,v0,-15416
  401324:	8fa3006c 	lw	v1,108(sp)
  401328:	8c440004 	lw	a0,4(v0)
  40132c:	27a50040 	addiu	a1,sp,64
  401330:	0c100f94 	jal	403e50 <IMP_OSD_UpdateRgnAttrData@plt>
  401334:	afa30040 	sw	v1,64(sp)
  401338:	0c100ee8 	jal	403ba0 <sleep@plt>
  40133c:	24040001 	li	a0,1
  401340:	1000ff86 	b	40115c <timestamp_update_func+0xbc>
  401344:	00000000 	nop
  401348:	24100001 	li	s0,1
  40134c:	3c110040 	lui	s1,0x40
  401350:	3c120040 	lui	s2,0x40
  401354:	3c050040 	lui	a1,0x40
  401358:	2646310c 	addiu	a2,s2,12556
  40135c:	240700a8 	li	a3,168
  401360:	262430f0 	addiu	a0,s1,12528
  401364:	0c100ecc 	jal	403b30 <printf@plt>
  401368:	24a533a4 	addiu	a1,a1,13220
  40136c:	3c040040 	lui	a0,0x40
  401370:	02002821 	move	a1,s0
  401374:	24843114 	addiu	a0,a0,12564
  401378:	0c100ecc 	jal	403b30 <printf@plt>
  40137c:	3c100040 	lui	s0,0x40
  401380:	0c100ecc 	jal	403b30 <printf@plt>
  401384:	2604313c 	addiu	a0,s0,12604
  401388:	3c050040 	lui	a1,0x40
  40138c:	24a5338c 	addiu	a1,a1,13196
  401390:	2646310c 	addiu	a2,s2,12556
  401394:	240700dc 	li	a3,220
  401398:	0c100ecc 	jal	403b30 <printf@plt>
  40139c:	262430f0 	addiu	a0,s1,12528
  4013a0:	3c040040 	lui	a0,0x40
  4013a4:	0c100f1c 	jal	403c70 <puts@plt>
  4013a8:	24843144 	addiu	a0,a0,12612
  4013ac:	0c100ecc 	jal	403b30 <printf@plt>
  4013b0:	2604313c 	addiu	a0,s0,12604
  4013b4:	0c100f5c 	jal	403d70 <pthread_exit@plt>
  4013b8:	00002021 	move	a0,zero
  4013bc:	1000ffe3 	b	40134c <timestamp_update_func+0x2ac>
  4013c0:	00008021 	move	s0,zero
  4013c4:	3c050040 	lui	a1,0x40
  4013c8:	3c060040 	lui	a2,0x40
  4013cc:	3c040040 	lui	a0,0x40
  4013d0:	24a5338c 	addiu	a1,a1,13196
  4013d4:	24c6310c 	addiu	a2,a2,12556
  4013d8:	240700d3 	li	a3,211
  4013dc:	0c100ecc 	jal	403b30 <printf@plt>
  4013e0:	248430f0 	addiu	a0,a0,12528
  4013e4:	3c040040 	lui	a0,0x40
  4013e8:	0c100f1c 	jal	403c70 <puts@plt>
  4013ec:	24843168 	addiu	a0,a0,12648
  4013f0:	3c040040 	lui	a0,0x40
  4013f4:	0c100ecc 	jal	403b30 <printf@plt>
  4013f8:	2484313c 	addiu	a0,a0,12604
  4013fc:	0c100ed0 	jal	403b40 <free@plt>
  401400:	8fa40068 	lw	a0,104(sp)
  401404:	0c100f5c 	jal	403d70 <pthread_exit@plt>
  401408:	00002021 	move	a0,zero
  40140c:	3c050040 	lui	a1,0x40
  401410:	3c060040 	lui	a2,0x40
  401414:	3c040040 	lui	a0,0x40
  401418:	24a5338c 	addiu	a1,a1,13196
  40141c:	24c6310c 	addiu	a2,a2,12556
  401420:	240700d3 	li	a3,211
  401424:	0c100ecc 	jal	403b30 <printf@plt>
  401428:	248430f0 	addiu	a0,a0,12528
  40142c:	3c040040 	lui	a0,0x40
  401430:	0c100f1c 	jal	403c70 <puts@plt>
  401434:	24843168 	addiu	a0,a0,12648
  401438:	3c040040 	lui	a0,0x40
  40143c:	0c100ecc 	jal	403b30 <printf@plt>
  401440:	2484313c 	addiu	a0,a0,12604
  401444:	8fa3006c 	lw	v1,108(sp)
  401448:	1060ffee 	beqz	v1,401404 <timestamp_update_func+0x364>
  40144c:	00000000 	nop
  401450:	0c100ed0 	jal	403b40 <free@plt>
  401454:	8fa4006c 	lw	a0,108(sp)
  401458:	1000ffea 	b	401404 <timestamp_update_func+0x364>
  40145c:	00000000 	nop

00401460 <osd_init>:
  401460:	27bdffa8 	addiu	sp,sp,-88
  401464:	afb1003c 	sw	s1,60(sp)
  401468:	afb00038 	sw	s0,56(sp)
  40146c:	3c110042 	lui	s1,0x42
  401470:	3c100041 	lui	s0,0x41
  401474:	afb60050 	sw	s6,80(sp)
  401478:	afb5004c 	sw	s5,76(sp)
  40147c:	afb40048 	sw	s4,72(sp)
  401480:	afb30044 	sw	s3,68(sp)
  401484:	afb20040 	sw	s2,64(sp)
  401488:	afbf0054 	sw	ra,84(sp)
  40148c:	26104100 	addiu	s0,s0,16640
  401490:	2631c3c8 	addiu	s1,s1,-15416
  401494:	00009021 	move	s2,zero
  401498:	2414ffff 	li	s4,-1
  40149c:	24150005 	li	s5,5
  4014a0:	2413000a 	li	s3,10
  4014a4:	24160149 	li	s6,329
  4014a8:	0c100f0c 	jal	403c30 <IMP_OSD_CreateGroup@plt>
  4014ac:	8e040000 	lw	a0,0(s0)
  4014b0:	04400031 	bltz	v0,401578 <osd_init+0x118>
  4014b4:	00002021 	move	a0,zero
  4014b8:	0c100f38 	jal	403ce0 <IMP_OSD_CreateRgn@plt>
  4014bc:	00000000 	nop
  4014c0:	00402021 	move	a0,v0
  4014c4:	00003021 	move	a2,zero
  4014c8:	10540045 	beq	v0,s4,4015e0 <osd_init+0x180>
  4014cc:	ae220000 	sw	v0,0(s1)
  4014d0:	0c100f8c 	jal	403e30 <IMP_OSD_RegisterRgn@plt>
  4014d4:	8e050000 	lw	a1,0(s0)
  4014d8:	0440004c 	bltz	v0,40160c <osd_init+0x1ac>
  4014dc:	3c060040 	lui	a2,0x40
  4014e0:	afa00034 	sw	zero,52(sp)
  4014e4:	afb50018 	sw	s5,24(sp)
  4014e8:	afb3001c 	sw	s3,28(sp)
  4014ec:	1640001e 	bnez	s2,401568 <osd_init+0x108>
  4014f0:	afb30020 	sw	s3,32(sp)
  4014f4:	2402002b 	li	v0,43
  4014f8:	afa20028 	sw	v0,40(sp)
  4014fc:	afb60024 	sw	s6,36(sp)
  401500:	8e240000 	lw	a0,0(s1)
  401504:	24020011 	li	v0,17
  401508:	27a50018 	addiu	a1,sp,24
  40150c:	afa2002c 	sw	v0,44(sp)
  401510:	0c100f3c 	jal	403cf0 <IMP_OSD_SetRgnAttr@plt>
  401514:	afa00030 	sw	zero,48(sp)
  401518:	04400046 	bltz	v0,401634 <osd_init+0x1d4>
  40151c:	26310004 	addiu	s1,s1,4
  401520:	8e040000 	lw	a0,0(s0)
  401524:	0c100f68 	jal	403da0 <IMP_OSD_Start@plt>
  401528:	26100004 	addiu	s0,s0,4
  40152c:	0440004c 	bltz	v0,401660 <osd_init+0x200>
  401530:	24020002 	li	v0,2
  401534:	26520001 	addiu	s2,s2,1
  401538:	1642ffdb 	bne	s2,v0,4014a8 <osd_init+0x48>
  40153c:	8fbf0054 	lw	ra,84(sp)
  401540:	8fb60050 	lw	s6,80(sp)
  401544:	8fb5004c 	lw	s5,76(sp)
  401548:	8fb40048 	lw	s4,72(sp)
  40154c:	8fb30044 	lw	s3,68(sp)
  401550:	8fb20040 	lw	s2,64(sp)
  401554:	8fb1003c 	lw	s1,60(sp)
  401558:	8fb00038 	lw	s0,56(sp)
  40155c:	00001021 	move	v0,zero
  401560:	03e00008 	jr	ra
  401564:	27bd0058 	addiu	sp,sp,88
  401568:	2402002b 	li	v0,43
  40156c:	afb60024 	sw	s6,36(sp)
  401570:	1000ffe3 	b	401500 <osd_init+0xa0>
  401574:	afa20028 	sw	v0,40(sp)
  401578:	3c060040 	lui	a2,0x40
  40157c:	3c040040 	lui	a0,0x40
  401580:	3c050040 	lui	a1,0x40
  401584:	248430f0 	addiu	a0,a0,12528
  401588:	24c6310c 	addiu	a2,a2,12556
  40158c:	24070033 	li	a3,51
  401590:	0c100ecc 	jal	403b30 <printf@plt>
  401594:	24a5335c 	addiu	a1,a1,13148
  401598:	3c040040 	lui	a0,0x40
  40159c:	24843184 	addiu	a0,a0,12676
  4015a0:	0c100ecc 	jal	403b30 <printf@plt>
  4015a4:	02402821 	move	a1,s2
  4015a8:	3c040040 	lui	a0,0x40
  4015ac:	0c100ecc 	jal	403b30 <printf@plt>
  4015b0:	2484313c 	addiu	a0,a0,12604
  4015b4:	8fbf0054 	lw	ra,84(sp)
  4015b8:	8fb60050 	lw	s6,80(sp)
  4015bc:	8fb5004c 	lw	s5,76(sp)
  4015c0:	8fb40048 	lw	s4,72(sp)
  4015c4:	8fb30044 	lw	s3,68(sp)
  4015c8:	8fb20040 	lw	s2,64(sp)
  4015cc:	8fb1003c 	lw	s1,60(sp)
  4015d0:	8fb00038 	lw	s0,56(sp)
  4015d4:	2402ffff 	li	v0,-1
  4015d8:	03e00008 	jr	ra
  4015dc:	27bd0058 	addiu	sp,sp,88
  4015e0:	3c060040 	lui	a2,0x40
  4015e4:	3c040040 	lui	a0,0x40
  4015e8:	3c050040 	lui	a1,0x40
  4015ec:	248430f0 	addiu	a0,a0,12528
  4015f0:	24c6310c 	addiu	a2,a2,12556
  4015f4:	24070039 	li	a3,57
  4015f8:	0c100ecc 	jal	403b30 <printf@plt>
  4015fc:	24a5335c 	addiu	a1,a1,13148
  401600:	3c040040 	lui	a0,0x40
  401604:	1000ffe6 	b	4015a0 <osd_init+0x140>
  401608:	248431a8 	addiu	a0,a0,12712
  40160c:	3c040040 	lui	a0,0x40
  401610:	3c050040 	lui	a1,0x40
  401614:	248430f0 	addiu	a0,a0,12528
  401618:	24c6310c 	addiu	a2,a2,12556
  40161c:	2407003f 	li	a3,63
  401620:	0c100ecc 	jal	403b30 <printf@plt>
  401624:	24a5335c 	addiu	a1,a1,13148
  401628:	3c040040 	lui	a0,0x40
  40162c:	1000ffdc 	b	4015a0 <osd_init+0x140>
  401630:	248431d4 	addiu	a0,a0,12756
  401634:	3c060040 	lui	a2,0x40
  401638:	3c040040 	lui	a0,0x40
  40163c:	3c050040 	lui	a1,0x40
  401640:	248430f0 	addiu	a0,a0,12528
  401644:	24c6310c 	addiu	a2,a2,12556
  401648:	2407005c 	li	a3,92
  40164c:	0c100ecc 	jal	403b30 <printf@plt>
  401650:	24a5335c 	addiu	a1,a1,13148
  401654:	3c040040 	lui	a0,0x40
  401658:	1000ffd1 	b	4015a0 <osd_init+0x140>
  40165c:	248431f8 	addiu	a0,a0,12792
  401660:	3c060040 	lui	a2,0x40
  401664:	3c040040 	lui	a0,0x40
  401668:	3c050040 	lui	a1,0x40
  40166c:	248430f0 	addiu	a0,a0,12528
  401670:	24c6310c 	addiu	a2,a2,12556
  401674:	24070063 	li	a3,99
  401678:	0c100ecc 	jal	403b30 <printf@plt>
  40167c:	24a5335c 	addiu	a1,a1,13148
  401680:	3c040040 	lui	a0,0x40
  401684:	1000ffc6 	b	4015a0 <osd_init+0x140>
  401688:	24843224 	addiu	a0,a0,12836

0040168c <osd_exit>:
  40168c:	27bdffc0 	addiu	sp,sp,-64
  401690:	afb0001c 	sw	s0,28(sp)
  401694:	3c100042 	lui	s0,0x42
  401698:	afb10020 	sw	s1,32(sp)
  40169c:	2610c3c8 	addiu	s0,s0,-15416
  4016a0:	3c110041 	lui	s1,0x41
  4016a4:	3c040040 	lui	a0,0x40
  4016a8:	3c050040 	lui	a1,0x40
  4016ac:	3c020040 	lui	v0,0x40
  4016b0:	afb70038 	sw	s7,56(sp)
  4016b4:	afb60034 	sw	s6,52(sp)
  4016b8:	afb50030 	sw	s5,48(sp)
  4016bc:	afb4002c 	sw	s4,44(sp)
  4016c0:	afb30028 	sw	s3,40(sp)
  4016c4:	afb20024 	sw	s2,36(sp)
  4016c8:	afbf003c 	sw	ra,60(sp)
  4016cc:	26314100 	addiu	s1,s1,16640
  4016d0:	26120008 	addiu	s2,s0,8
  4016d4:	249630f0 	addiu	s6,a0,12528
  4016d8:	24b53368 	addiu	s5,a1,13160
  4016dc:	3c140040 	lui	s4,0x40
  4016e0:	2457324c 	addiu	s7,v0,12876
  4016e4:	3c130040 	lui	s3,0x40
  4016e8:	8e040000 	lw	a0,0(s0)
  4016ec:	8e250000 	lw	a1,0(s1)
  4016f0:	0c100f64 	jal	403d90 <IMP_OSD_ShowRgn@plt>
  4016f4:	00003021 	move	a2,zero
  4016f8:	0440001b 	bltz	v0,401768 <osd_exit+0xdc>
  4016fc:	2686310c 	addiu	a2,s4,12556
  401700:	8e250000 	lw	a1,0(s1)
  401704:	0c100ec0 	jal	403b00 <IMP_OSD_UnRegisterRgn@plt>
  401708:	8e040000 	lw	a0,0(s0)
  40170c:	04400023 	bltz	v0,40179c <osd_exit+0x110>
  401710:	02a02821 	move	a1,s5
  401714:	8e040000 	lw	a0,0(s0)
  401718:	26310004 	addiu	s1,s1,4
  40171c:	0c100f10 	jal	403c40 <IMP_OSD_DestroyRgn@plt>
  401720:	26100004 	addiu	s0,s0,4
  401724:	0c100f6c 	jal	403db0 <IMP_OSD_DestroyGroup@plt>
  401728:	8e24fffc 	lw	a0,-4(s1)
  40172c:	04400026 	bltz	v0,4017c8 <osd_exit+0x13c>
  401730:	3c050040 	lui	a1,0x40
  401734:	1612ffec 	bne	s0,s2,4016e8 <osd_exit+0x5c>
  401738:	00001021 	move	v0,zero
  40173c:	8fbf003c 	lw	ra,60(sp)
  401740:	8fb70038 	lw	s7,56(sp)
  401744:	8fb60034 	lw	s6,52(sp)
  401748:	8fb50030 	lw	s5,48(sp)
  40174c:	8fb4002c 	lw	s4,44(sp)
  401750:	8fb30028 	lw	s3,40(sp)
  401754:	8fb20024 	lw	s2,36(sp)
  401758:	8fb10020 	lw	s1,32(sp)
  40175c:	8fb0001c 	lw	s0,28(sp)
  401760:	03e00008 	jr	ra
  401764:	27bd0040 	addiu	sp,sp,64
  401768:	24070080 	li	a3,128
  40176c:	02a02821 	move	a1,s5
  401770:	0c100ecc 	jal	403b30 <printf@plt>
  401774:	02c02021 	move	a0,s6
  401778:	0c100f1c 	jal	403c70 <puts@plt>
  40177c:	02e02021 	move	a0,s7
  401780:	0c100ecc 	jal	403b30 <printf@plt>
  401784:	2664313c 	addiu	a0,s3,12604
  401788:	8e250000 	lw	a1,0(s1)
  40178c:	0c100ec0 	jal	403b00 <IMP_OSD_UnRegisterRgn@plt>
  401790:	8e040000 	lw	a0,0(s0)
  401794:	0441ffdf 	bgez	v0,401714 <osd_exit+0x88>
  401798:	02a02821 	move	a1,s5
  40179c:	2686310c 	addiu	a2,s4,12556
  4017a0:	24070085 	li	a3,133
  4017a4:	0c100ecc 	jal	403b30 <printf@plt>
  4017a8:	02c02021 	move	a0,s6
  4017ac:	3c040040 	lui	a0,0x40
  4017b0:	0c100f1c 	jal	403c70 <puts@plt>
  4017b4:	24843274 	addiu	a0,a0,12916
  4017b8:	0c100ecc 	jal	403b30 <printf@plt>
  4017bc:	2664313c 	addiu	a0,s3,12604
  4017c0:	1000ffd5 	b	401718 <osd_exit+0x8c>
  4017c4:	8e040000 	lw	a0,0(s0)
  4017c8:	3c060040 	lui	a2,0x40
  4017cc:	3c040040 	lui	a0,0x40
  4017d0:	24a53368 	addiu	a1,a1,13160
  4017d4:	24c6310c 	addiu	a2,a2,12556
  4017d8:	2407008d 	li	a3,141
  4017dc:	0c100ecc 	jal	403b30 <printf@plt>
  4017e0:	248430f0 	addiu	a0,a0,12528
  4017e4:	3c040040 	lui	a0,0x40
  4017e8:	0c100f1c 	jal	403c70 <puts@plt>
  4017ec:	2484329c 	addiu	a0,a0,12956
  4017f0:	3c040040 	lui	a0,0x40
  4017f4:	0c100ecc 	jal	403b30 <printf@plt>
  4017f8:	2484313c 	addiu	a0,a0,12604
  4017fc:	1000ffcf 	b	40173c <osd_exit+0xb0>
  401800:	2402ffff 	li	v0,-1

00401804 <timestamp_update_task>:
  401804:	27bdffd8 	addiu	sp,sp,-40
  401808:	3c060040 	lui	a2,0x40
  40180c:	27a40018 	addiu	a0,sp,24
  401810:	00002821 	move	a1,zero
  401814:	24c610a0 	addiu	a2,a2,4256
  401818:	afbf0024 	sw	ra,36(sp)
  40181c:	0c100f14 	jal	403c50 <pthread_create@plt>
  401820:	00003821 	move	a3,zero
  401824:	14400005 	bnez	v0,40183c <timestamp_update_task+0x38>
  401828:	00001821 	move	v1,zero
  40182c:	8fbf0024 	lw	ra,36(sp)
  401830:	00601021 	move	v0,v1
  401834:	03e00008 	jr	ra
  401838:	27bd0028 	addiu	sp,sp,40
  40183c:	3c050040 	lui	a1,0x40
  401840:	3c060040 	lui	a2,0x40
  401844:	3c040040 	lui	a0,0x40
  401848:	24a53374 	addiu	a1,a1,13172
  40184c:	24c6310c 	addiu	a2,a2,12556
  401850:	24070137 	li	a3,311
  401854:	0c100ecc 	jal	403b30 <printf@plt>
  401858:	248430f0 	addiu	a0,a0,12528
  40185c:	3c040040 	lui	a0,0x40
  401860:	0c100f1c 	jal	403c70 <puts@plt>
  401864:	248432bc 	addiu	a0,a0,12988
  401868:	3c040040 	lui	a0,0x40
  40186c:	0c100ecc 	jal	403b30 <printf@plt>
  401870:	2484313c 	addiu	a0,a0,12604
  401874:	1000ffed 	b	40182c <timestamp_update_task+0x28>
  401878:	2403ffff 	li	v1,-1
  40187c:	00000000 	nop

00401880 <save_stream>:
  401880:	27bdffd0 	addiu	sp,sp,-48
  401884:	afb30024 	sw	s3,36(sp)
  401888:	8cb30004 	lw	s3,4(a1)
  40188c:	afbf002c 	sw	ra,44(sp)
  401890:	afb40028 	sw	s4,40(sp)
  401894:	afb20020 	sw	s2,32(sp)
  401898:	afb1001c 	sw	s1,28(sp)
  40189c:	1a60002f 	blez	s3,40195c <save_stream+0xdc>
  4018a0:	afb00018 	sw	s0,24(sp)
  4018a4:	00a09021 	move	s2,a1
  4018a8:	0080a021 	move	s4,a0
  4018ac:	8ca30000 	lw	v1,0(a1)
  4018b0:	10000003 	b	4018c0 <save_stream+0x40>
  4018b4:	00008021 	move	s0,zero
  4018b8:	12700029 	beq	s3,s0,401960 <save_stream+0xe0>
  4018bc:	8fbf002c 	lw	ra,44(sp)
  4018c0:	00108940 	sll	s1,s0,0x5
  4018c4:	00711821 	addu	v1,v1,s1
  4018c8:	8c660008 	lw	a2,8(v1)
  4018cc:	8c650004 	lw	a1,4(v1)
  4018d0:	02802021 	move	a0,s4
  4018d4:	0c100f44 	jal	403d10 <write@plt>
  4018d8:	26100001 	addiu	s0,s0,1
  4018dc:	8e430000 	lw	v1,0(s2)
  4018e0:	00718821 	addu	s1,v1,s1
  4018e4:	8e260008 	lw	a2,8(s1)
  4018e8:	10c2fff3 	beq	a2,v0,4018b8 <save_stream+0x38>
  4018ec:	3c060040 	lui	a2,0x40
  4018f0:	3c050040 	lui	a1,0x40
  4018f4:	3c040040 	lui	a0,0x40
  4018f8:	24c633b0 	addiu	a2,a2,13232
  4018fc:	240700a8 	li	a3,168
  401900:	24a53800 	addiu	a1,a1,14336
  401904:	0c100ecc 	jal	403b30 <printf@plt>
  401908:	248430f0 	addiu	a0,a0,12528
  40190c:	0c100f4c 	jal	403d30 <__errno_location@plt>
  401910:	00000000 	nop
  401914:	0c100f20 	jal	403c80 <strerror@plt>
  401918:	8c440000 	lw	a0,0(v0)
  40191c:	3c040040 	lui	a0,0x40
  401920:	00402821 	move	a1,v0
  401924:	0c100ecc 	jal	403b30 <printf@plt>
  401928:	248433b8 	addiu	a0,a0,13240
  40192c:	3c040040 	lui	a0,0x40
  401930:	0c100ecc 	jal	403b30 <printf@plt>
  401934:	2484313c 	addiu	a0,a0,12604
  401938:	8fbf002c 	lw	ra,44(sp)
  40193c:	8fb40028 	lw	s4,40(sp)
  401940:	8fb30024 	lw	s3,36(sp)
  401944:	8fb20020 	lw	s2,32(sp)
  401948:	8fb1001c 	lw	s1,28(sp)
  40194c:	8fb00018 	lw	s0,24(sp)
  401950:	2402ffff 	li	v0,-1
  401954:	03e00008 	jr	ra
  401958:	27bd0030 	addiu	sp,sp,48
  40195c:	8fbf002c 	lw	ra,44(sp)
  401960:	8fb40028 	lw	s4,40(sp)
  401964:	8fb30024 	lw	s3,36(sp)
  401968:	8fb20020 	lw	s2,32(sp)
  40196c:	8fb1001c 	lw	s1,28(sp)
  401970:	8fb00018 	lw	s0,24(sp)
  401974:	00001021 	move	v0,zero
  401978:	03e00008 	jr	ra
  40197c:	27bd0030 	addiu	sp,sp,48

00401980 <encoder_chn_exit>:
  401980:	27bdffc0 	addiu	sp,sp,-64
  401984:	27a50018 	addiu	a1,sp,24
  401988:	afb10034 	sw	s1,52(sp)
  40198c:	afbf003c 	sw	ra,60(sp)
  401990:	afb20038 	sw	s2,56(sp)
  401994:	afb00030 	sw	s0,48(sp)
  401998:	0c100ef0 	jal	403bc0 <IMP_Encoder_Query@plt>
  40199c:	00808821 	move	s1,a0
  4019a0:	04400026 	bltz	v0,401a3c <encoder_chn_exit+0xbc>
  4019a4:	3c040040 	lui	a0,0x40
  4019a8:	93a20018 	lbu	v0,24(sp)
  4019ac:	14400008 	bnez	v0,4019d0 <encoder_chn_exit+0x50>
  4019b0:	00008021 	move	s0,zero
  4019b4:	8fbf003c 	lw	ra,60(sp)
  4019b8:	02001021 	move	v0,s0
  4019bc:	8fb20038 	lw	s2,56(sp)
  4019c0:	8fb10034 	lw	s1,52(sp)
  4019c4:	8fb00030 	lw	s0,48(sp)
  4019c8:	03e00008 	jr	ra
  4019cc:	27bd0040 	addiu	sp,sp,64
  4019d0:	0c100f60 	jal	403d80 <IMP_Encoder_UnRegisterChn@plt>
  4019d4:	02202021 	move	a0,s1
  4019d8:	0440002b 	bltz	v0,401a88 <encoder_chn_exit+0x108>
  4019dc:	00409021 	move	s2,v0
  4019e0:	0c100f50 	jal	403d40 <IMP_Encoder_DestroyChn@plt>
  4019e4:	02202021 	move	a0,s1
  4019e8:	0441fff2 	bgez	v0,4019b4 <encoder_chn_exit+0x34>
  4019ec:	00409021 	move	s2,v0
  4019f0:	3c040040 	lui	a0,0x40
  4019f4:	3c050040 	lui	a1,0x40
  4019f8:	3c060040 	lui	a2,0x40
  4019fc:	2407020a 	li	a3,522
  401a00:	248430f0 	addiu	a0,a0,12528
  401a04:	24a53840 	addiu	a1,a1,14400
  401a08:	0c100ecc 	jal	403b30 <printf@plt>
  401a0c:	24c633b0 	addiu	a2,a2,13232
  401a10:	3c040040 	lui	a0,0x40
  401a14:	24843420 	addiu	a0,a0,13344
  401a18:	02202821 	move	a1,s1
  401a1c:	0c100ecc 	jal	403b30 <printf@plt>
  401a20:	02403021 	move	a2,s2
  401a24:	3c040040 	lui	a0,0x40
  401a28:	2484313c 	addiu	a0,a0,12604
  401a2c:	0c100ecc 	jal	403b30 <printf@plt>
  401a30:	2410ffff 	li	s0,-1
  401a34:	1000ffe0 	b	4019b8 <encoder_chn_exit+0x38>
  401a38:	8fbf003c 	lw	ra,60(sp)
  401a3c:	3c050040 	lui	a1,0x40
  401a40:	3c060040 	lui	a2,0x40
  401a44:	248430f0 	addiu	a0,a0,12528
  401a48:	24a53840 	addiu	a1,a1,14400
  401a4c:	24c633b0 	addiu	a2,a2,13232
  401a50:	240701fb 	li	a3,507
  401a54:	0c100ecc 	jal	403b30 <printf@plt>
  401a58:	00408021 	move	s0,v0
  401a5c:	3c040040 	lui	a0,0x40
  401a60:	248433d0 	addiu	a0,a0,13264
  401a64:	02202821 	move	a1,s1
  401a68:	02003021 	move	a2,s0
  401a6c:	0c100ecc 	jal	403b30 <printf@plt>
  401a70:	2410ffff 	li	s0,-1
  401a74:	3c040040 	lui	a0,0x40
  401a78:	0c100ecc 	jal	403b30 <printf@plt>
  401a7c:	2484313c 	addiu	a0,a0,12604
  401a80:	1000ffcd 	b	4019b8 <encoder_chn_exit+0x38>
  401a84:	8fbf003c 	lw	ra,60(sp)
  401a88:	3c040040 	lui	a0,0x40
  401a8c:	3c050040 	lui	a1,0x40
  401a90:	3c060040 	lui	a2,0x40
  401a94:	248430f0 	addiu	a0,a0,12528
  401a98:	24a53840 	addiu	a1,a1,14400
  401a9c:	24c633b0 	addiu	a2,a2,13232
  401aa0:	0c100ecc 	jal	403b30 <printf@plt>
  401aa4:	24070203 	li	a3,515
  401aa8:	3c040040 	lui	a0,0x40
  401aac:	248433f4 	addiu	a0,a0,13300
  401ab0:	02202821 	move	a1,s1
  401ab4:	1000ffed 	b	401a6c <encoder_chn_exit+0xec>
  401ab8:	02403021 	move	a2,s2

00401abc <video_start>:
  401abc:	27bdffd8 	addiu	sp,sp,-40
  401ac0:	2c820002 	sltiu	v0,a0,2
  401ac4:	afbf0024 	sw	ra,36(sp)
  401ac8:	afb10020 	sw	s1,32(sp)
  401acc:	10400022 	beqz	v0,401b58 <video_start+0x9c>
  401ad0:	afb0001c 	sw	s0,28(sp)
  401ad4:	3c100042 	lui	s0,0x42
  401ad8:	2602c2e0 	addiu	v0,s0,-15648
  401adc:	8c420004 	lw	v0,4(v0)
  401ae0:	14400007 	bnez	v0,401b00 <video_start+0x44>
  401ae4:	00000000 	nop
  401ae8:	00001021 	move	v0,zero
  401aec:	8fbf0024 	lw	ra,36(sp)
  401af0:	8fb10020 	lw	s1,32(sp)
  401af4:	8fb0001c 	lw	s0,28(sp)
  401af8:	03e00008 	jr	ra
  401afc:	27bd0028 	addiu	sp,sp,40
  401b00:	0c100efc 	jal	403bf0 <IMP_FrameSource_EnableChn@plt>
  401b04:	8e04c2e0 	lw	a0,-15648(s0)
  401b08:	0441fff7 	bgez	v0,401ae8 <video_start+0x2c>
  401b0c:	00408821 	move	s1,v0
  401b10:	3c040040 	lui	a0,0x40
  401b14:	3c050040 	lui	a1,0x40
  401b18:	3c060040 	lui	a2,0x40
  401b1c:	24070166 	li	a3,358
  401b20:	248430f0 	addiu	a0,a0,12528
  401b24:	24a537dc 	addiu	a1,a1,14300
  401b28:	0c100ecc 	jal	403b30 <printf@plt>
  401b2c:	24c633b0 	addiu	a2,a2,13232
  401b30:	8e06c2e0 	lw	a2,-15648(s0)
  401b34:	3c040040 	lui	a0,0x40
  401b38:	24843464 	addiu	a0,a0,13412
  401b3c:	0c100ecc 	jal	403b30 <printf@plt>
  401b40:	02202821 	move	a1,s1
  401b44:	3c040040 	lui	a0,0x40
  401b48:	0c100ecc 	jal	403b30 <printf@plt>
  401b4c:	2484313c 	addiu	a0,a0,12604
  401b50:	1000ffe6 	b	401aec <video_start+0x30>
  401b54:	2402ffff 	li	v0,-1
  401b58:	00808021 	move	s0,a0
  401b5c:	3c060040 	lui	a2,0x40
  401b60:	3c040040 	lui	a0,0x40
  401b64:	3c050040 	lui	a1,0x40
  401b68:	24c633b0 	addiu	a2,a2,13232
  401b6c:	2407015a 	li	a3,346
  401b70:	248430f0 	addiu	a0,a0,12528
  401b74:	0c100ecc 	jal	403b30 <printf@plt>
  401b78:	24a537dc 	addiu	a1,a1,14300
  401b7c:	3c040040 	lui	a0,0x40
  401b80:	24843448 	addiu	a0,a0,13384
  401b84:	0c100ecc 	jal	403b30 <printf@plt>
  401b88:	02002821 	move	a1,s0
  401b8c:	3c040040 	lui	a0,0x40
  401b90:	0c100ecc 	jal	403b30 <printf@plt>
  401b94:	2484313c 	addiu	a0,a0,12604
  401b98:	1000ffd4 	b	401aec <video_start+0x30>
  401b9c:	2402ffff 	li	v0,-1

00401ba0 <video_get_h264_stream_task>:
  401ba0:	27bdffc0 	addiu	sp,sp,-64
  401ba4:	3c020042 	lui	v0,0x42
  401ba8:	3c060040 	lui	a2,0x40
  401bac:	afb10024 	sw	s1,36(sp)
  401bb0:	3c050040 	lui	a1,0x40
  401bb4:	3c110042 	lui	s1,0x42
  401bb8:	afb60038 	sw	s6,56(sp)
  401bbc:	afb50034 	sw	s5,52(sp)
  401bc0:	afb40030 	sw	s4,48(sp)
  401bc4:	afb3002c 	sw	s3,44(sp)
  401bc8:	afb20028 	sw	s2,40(sp)
  401bcc:	afb00020 	sw	s0,32(sp)
  401bd0:	afbf003c 	sw	ra,60(sp)
  401bd4:	2631c2e4 	addiu	s1,s1,-15644
  401bd8:	00008021 	move	s0,zero
  401bdc:	2456c2e0 	addiu	s6,v0,-15648
  401be0:	24d21d78 	addiu	s2,a2,7544
  401be4:	3c140040 	lui	s4,0x40
  401be8:	24b5380c 	addiu	s5,a1,14348
  401bec:	3c130040 	lui	s3,0x40
  401bf0:	8e220000 	lw	v0,0(s1)
  401bf4:	14400010 	bnez	v0,401c38 <video_get_h264_stream_task+0x98>
  401bf8:	00102080 	sll	a0,s0,0x2
  401bfc:	26100001 	addiu	s0,s0,1
  401c00:	24020002 	li	v0,2
  401c04:	1602fffa 	bne	s0,v0,401bf0 <video_get_h264_stream_task+0x50>
  401c08:	26310068 	addiu	s1,s1,104
  401c0c:	8fbf003c 	lw	ra,60(sp)
  401c10:	8fb60038 	lw	s6,56(sp)
  401c14:	8fb50034 	lw	s5,52(sp)
  401c18:	8fb40030 	lw	s4,48(sp)
  401c1c:	8fb3002c 	lw	s3,44(sp)
  401c20:	8fb20028 	lw	s2,40(sp)
  401c24:	8fb10024 	lw	s1,36(sp)
  401c28:	8fb00020 	lw	s0,32(sp)
  401c2c:	00001021 	move	v0,zero
  401c30:	03e00008 	jr	ra
  401c34:	27bd0040 	addiu	sp,sp,64
  401c38:	00103900 	sll	a3,s0,0x4
  401c3c:	00e43823 	subu	a3,a3,a0
  401c40:	00f03821 	addu	a3,a3,s0
  401c44:	27a20018 	addiu	v0,sp,24
  401c48:	000738c0 	sll	a3,a3,0x3
  401c4c:	00442021 	addu	a0,v0,a0
  401c50:	00002821 	move	a1,zero
  401c54:	02403021 	move	a2,s2
  401c58:	0c100f14 	jal	403c50 <pthread_create@plt>
  401c5c:	02c73821 	addu	a3,s6,a3
  401c60:	0441ffe6 	bgez	v0,401bfc <video_get_h264_stream_task+0x5c>
  401c64:	266633b0 	addiu	a2,s3,13232
  401c68:	240701d2 	li	a3,466
  401c6c:	268430f0 	addiu	a0,s4,12528
  401c70:	0c100ecc 	jal	403b30 <printf@plt>
  401c74:	02a02821 	move	a1,s5
  401c78:	8e25fffc 	lw	a1,-4(s1)
  401c7c:	3c040040 	lui	a0,0x40
  401c80:	0c100ecc 	jal	403b30 <printf@plt>
  401c84:	24843490 	addiu	a0,a0,13456
  401c88:	3c040040 	lui	a0,0x40
  401c8c:	0c100ecc 	jal	403b30 <printf@plt>
  401c90:	2484313c 	addiu	a0,a0,12604
  401c94:	1000ffda 	b	401c00 <video_get_h264_stream_task+0x60>
  401c98:	26100001 	addiu	s0,s0,1

00401c9c <video_stop>:
  401c9c:	27bdffd8 	addiu	sp,sp,-40
  401ca0:	3c020042 	lui	v0,0x42
  401ca4:	afb00018 	sw	s0,24(sp)
  401ca8:	2450c2e0 	addiu	s0,v0,-15648
  401cac:	8e030004 	lw	v1,4(s0)
  401cb0:	afbf0024 	sw	ra,36(sp)
  401cb4:	afb20020 	sw	s2,32(sp)
  401cb8:	14600012 	bnez	v1,401d04 <video_stop+0x68>
  401cbc:	afb1001c 	sw	s1,28(sp)
  401cc0:	8e02006c 	lw	v0,108(s0)
  401cc4:	14400008 	bnez	v0,401ce8 <video_stop+0x4c>
  401cc8:	00000000 	nop
  401ccc:	00001021 	move	v0,zero
  401cd0:	8fbf0024 	lw	ra,36(sp)
  401cd4:	8fb20020 	lw	s2,32(sp)
  401cd8:	8fb1001c 	lw	s1,28(sp)
  401cdc:	8fb00018 	lw	s0,24(sp)
  401ce0:	03e00008 	jr	ra
  401ce4:	27bd0028 	addiu	sp,sp,40
  401ce8:	8e040068 	lw	a0,104(s0)
  401cec:	0c100ed4 	jal	403b50 <IMP_FrameSource_DisableChn@plt>
  401cf0:	24120001 	li	s2,1
  401cf4:	0441fff5 	bgez	v0,401ccc <video_stop+0x30>
  401cf8:	00408821 	move	s1,v0
  401cfc:	10000007 	b	401d1c <video_stop+0x80>
  401d00:	3c040040 	lui	a0,0x40
  401d04:	0c100ed4 	jal	403b50 <IMP_FrameSource_DisableChn@plt>
  401d08:	8c44c2e0 	lw	a0,-15648(v0)
  401d0c:	0441ffec 	bgez	v0,401cc0 <video_stop+0x24>
  401d10:	00408821 	move	s1,v0
  401d14:	00009021 	move	s2,zero
  401d18:	3c040040 	lui	a0,0x40
  401d1c:	3c050040 	lui	a1,0x40
  401d20:	3c060040 	lui	a2,0x40
  401d24:	240701eb 	li	a3,491
  401d28:	248430f0 	addiu	a0,a0,12528
  401d2c:	24a53828 	addiu	a1,a1,14376
  401d30:	0c100ecc 	jal	403b30 <printf@plt>
  401d34:	24c633b0 	addiu	a2,a2,13232
  401d38:	00121900 	sll	v1,s2,0x4
  401d3c:	00121080 	sll	v0,s2,0x2
  401d40:	00621023 	subu	v0,v1,v0
  401d44:	00521021 	addu	v0,v0,s2
  401d48:	000210c0 	sll	v0,v0,0x3
  401d4c:	02028021 	addu	s0,s0,v0
  401d50:	8e060000 	lw	a2,0(s0)
  401d54:	3c040040 	lui	a0,0x40
  401d58:	248434b4 	addiu	a0,a0,13492
  401d5c:	0c100ecc 	jal	403b30 <printf@plt>
  401d60:	02202821 	move	a1,s1
  401d64:	3c040040 	lui	a0,0x40
  401d68:	0c100ecc 	jal	403b30 <printf@plt>
  401d6c:	2484313c 	addiu	a0,a0,12604
  401d70:	1000ffd7 	b	401cd0 <video_stop+0x34>
  401d74:	2402ffff 	li	v0,-1

00401d78 <get_h264_stream_func>:
  401d78:	27bdff70 	addiu	sp,sp,-144
  401d7c:	afbf008c 	sw	ra,140(sp)
  401d80:	afb1006c 	sw	s1,108(sp)
  401d84:	afb00068 	sw	s0,104(sp)
  401d88:	afbe0088 	sw	s8,136(sp)
  401d8c:	00808021 	move	s0,a0
  401d90:	afb70084 	sw	s7,132(sp)
  401d94:	afb60080 	sw	s6,128(sp)
  401d98:	afb5007c 	sw	s5,124(sp)
  401d9c:	afb40078 	sw	s4,120(sp)
  401da0:	afb30074 	sw	s3,116(sp)
  401da4:	0c100edc 	jal	403b70 <pthread_self@plt>
  401da8:	afb20070 	sw	s2,112(sp)
  401dac:	0c100f90 	jal	403e40 <pthread_detach@plt>
  401db0:	00402021 	move	a0,v0
  401db4:	8e110000 	lw	s1,0(s0)
  401db8:	0c1006af 	jal	401abc <video_start>
  401dbc:	02202021 	move	a0,s1
  401dc0:	0c100f88 	jal	403e20 <IMP_Encoder_StartRecvPic@plt>
  401dc4:	02202021 	move	a0,s1
  401dc8:	0440007f 	bltz	v0,401fc8 <get_h264_stream_func+0x250>
  401dcc:	3c060040 	lui	a2,0x40
  401dd0:	3c050040 	lui	a1,0x40
  401dd4:	27a40018 	addiu	a0,sp,24
  401dd8:	24a53508 	addiu	a1,a1,13576
  401ddc:	24c6351c 	addiu	a2,a2,13596
  401de0:	02203821 	move	a3,s1
  401de4:	3c100040 	lui	s0,0x40
  401de8:	3c150040 	lui	s5,0x40
  401dec:	0c100f7c 	jal	403df0 <sprintf@plt>
  401df0:	3c140040 	lui	s4,0x40
  401df4:	268633b0 	addiu	a2,s4,13232
  401df8:	2407018d 	li	a3,397
  401dfc:	26043524 	addiu	a0,s0,13604
  401e00:	0c100ecc 	jal	403b30 <printf@plt>
  401e04:	26a537e8 	addiu	a1,s5,14312
  401e08:	3c040040 	lui	a0,0x40
  401e0c:	24843538 	addiu	a0,a0,13624
  401e10:	0c100ecc 	jal	403b30 <printf@plt>
  401e14:	27a50018 	addiu	a1,sp,24
  401e18:	27a40018 	addiu	a0,sp,24
  401e1c:	24050302 	li	a1,770
  401e20:	0c100f28 	jal	403ca0 <open@plt>
  401e24:	240601ff 	li	a2,511
  401e28:	04400075 	bltz	v0,402000 <get_h264_stream_func+0x288>
  401e2c:	00409821 	move	s3,v0
  401e30:	26043524 	addiu	a0,s0,13604
  401e34:	26a537e8 	addiu	a1,s5,14312
  401e38:	268633b0 	addiu	a2,s4,13232
  401e3c:	0c100ecc 	jal	403b30 <printf@plt>
  401e40:	24070193 	li	a3,403
  401e44:	3c040040 	lui	a0,0x40
  401e48:	2484355c 	addiu	a0,a0,13660
  401e4c:	3c1e0040 	lui	s8,0x40
  401e50:	3c170040 	lui	s7,0x40
  401e54:	0c100f1c 	jal	403c70 <puts@plt>
  401e58:	241003e8 	li	s0,1000
  401e5c:	27de30f0 	addiu	s8,s8,12528
  401e60:	26f73560 	addiu	s7,s7,13664
  401e64:	10000011 	b	401eac <get_h264_stream_func+0x134>
  401e68:	3c160040 	lui	s6,0x40
  401e6c:	02202021 	move	a0,s1
  401e70:	0c100f98 	jal	403e60 <IMP_Encoder_GetStream@plt>
  401e74:	24060001 	li	a2,1
  401e78:	0440002b 	bltz	v0,401f28 <get_h264_stream_func+0x1b0>
  401e7c:	3c040040 	lui	a0,0x40
  401e80:	02602021 	move	a0,s3
  401e84:	0c100620 	jal	401880 <save_stream>
  401e88:	27a50058 	addiu	a1,sp,88
  401e8c:	0440003f 	bltz	v0,401f8c <get_h264_stream_func+0x214>
  401e90:	00409021 	move	s2,v0
  401e94:	02202021 	move	a0,s1
  401e98:	27a50058 	addiu	a1,sp,88
  401e9c:	0c100f80 	jal	403e00 <IMP_Encoder_ReleaseStream@plt>
  401ea0:	2610ffff 	addiu	s0,s0,-1
  401ea4:	12000012 	beqz	s0,401ef0 <get_h264_stream_func+0x178>
  401ea8:	00000000 	nop
  401eac:	02202021 	move	a0,s1
  401eb0:	0c100fa4 	jal	403e90 <IMP_Encoder_PollingStream@plt>
  401eb4:	240503e8 	li	a1,1000
  401eb8:	0441ffec 	bgez	v0,401e6c <get_h264_stream_func+0xf4>
  401ebc:	27a50058 	addiu	a1,sp,88
  401ec0:	26a537e8 	addiu	a1,s5,14312
  401ec4:	268633b0 	addiu	a2,s4,13232
  401ec8:	2407019a 	li	a3,410
  401ecc:	0c100ecc 	jal	403b30 <printf@plt>
  401ed0:	03c02021 	move	a0,s8
  401ed4:	02e02021 	move	a0,s7
  401ed8:	0c100f1c 	jal	403c70 <puts@plt>
  401edc:	2610ffff 	addiu	s0,s0,-1
  401ee0:	0c100ecc 	jal	403b30 <printf@plt>
  401ee4:	26c4313c 	addiu	a0,s6,12604
  401ee8:	1600fff1 	bnez	s0,401eb0 <get_h264_stream_func+0x138>
  401eec:	02202021 	move	a0,s1
  401ef0:	0c100f9c 	jal	403e70 <close@plt>
  401ef4:	02602021 	move	a0,s3
  401ef8:	0c100fa0 	jal	403e80 <IMP_Encoder_StopRecvPic@plt>
  401efc:	02202021 	move	a0,s1
  401f00:	0441004f 	bgez	v0,402040 <get_h264_stream_func+0x2c8>
  401f04:	3c040040 	lui	a0,0x40
  401f08:	248430f0 	addiu	a0,a0,12528
  401f0c:	26a537e8 	addiu	a1,s5,14312
  401f10:	268633b0 	addiu	a2,s4,13232
  401f14:	0c100ecc 	jal	403b30 <printf@plt>
  401f18:	240701b6 	li	a3,438
  401f1c:	3c040040 	lui	a0,0x40
  401f20:	10000008 	b	401f44 <get_h264_stream_func+0x1cc>
  401f24:	24843598 	addiu	a0,a0,13720
  401f28:	248430f0 	addiu	a0,a0,12528
  401f2c:	26a537e8 	addiu	a1,s5,14312
  401f30:	268633b0 	addiu	a2,s4,13232
  401f34:	0c100ecc 	jal	403b30 <printf@plt>
  401f38:	240701a3 	li	a3,419
  401f3c:	3c040040 	lui	a0,0x40
  401f40:	24843578 	addiu	a0,a0,13688
  401f44:	0c100f1c 	jal	403c70 <puts@plt>
  401f48:	00000000 	nop
  401f4c:	3c040040 	lui	a0,0x40
  401f50:	0c100ecc 	jal	403b30 <printf@plt>
  401f54:	2484313c 	addiu	a0,a0,12604
  401f58:	8fbf008c 	lw	ra,140(sp)
  401f5c:	8fbe0088 	lw	s8,136(sp)
  401f60:	8fb70084 	lw	s7,132(sp)
  401f64:	8fb60080 	lw	s6,128(sp)
  401f68:	8fb5007c 	lw	s5,124(sp)
  401f6c:	8fb40078 	lw	s4,120(sp)
  401f70:	8fb30074 	lw	s3,116(sp)
  401f74:	8fb20070 	lw	s2,112(sp)
  401f78:	8fb1006c 	lw	s1,108(sp)
  401f7c:	8fb00068 	lw	s0,104(sp)
  401f80:	2402ffff 	li	v0,-1
  401f84:	03e00008 	jr	ra
  401f88:	27bd0090 	addiu	sp,sp,144
  401f8c:	0c100f9c 	jal	403e70 <close@plt>
  401f90:	02602021 	move	a0,s3
  401f94:	8fbf008c 	lw	ra,140(sp)
  401f98:	02401021 	move	v0,s2
  401f9c:	8fbe0088 	lw	s8,136(sp)
  401fa0:	8fb70084 	lw	s7,132(sp)
  401fa4:	8fb60080 	lw	s6,128(sp)
  401fa8:	8fb5007c 	lw	s5,124(sp)
  401fac:	8fb40078 	lw	s4,120(sp)
  401fb0:	8fb30074 	lw	s3,116(sp)
  401fb4:	8fb20070 	lw	s2,112(sp)
  401fb8:	8fb1006c 	lw	s1,108(sp)
  401fbc:	8fb00068 	lw	s0,104(sp)
  401fc0:	03e00008 	jr	ra
  401fc4:	27bd0090 	addiu	sp,sp,144
  401fc8:	3c060040 	lui	a2,0x40
  401fcc:	3c040040 	lui	a0,0x40
  401fd0:	3c050040 	lui	a1,0x40
  401fd4:	24c633b0 	addiu	a2,a2,13232
  401fd8:	24070185 	li	a3,389
  401fdc:	248430f0 	addiu	a0,a0,12528
  401fe0:	0c100ecc 	jal	403b30 <printf@plt>
  401fe4:	24a537e8 	addiu	a1,a1,14312
  401fe8:	3c040040 	lui	a0,0x40
  401fec:	248434e0 	addiu	a0,a0,13536
  401ff0:	0c100ecc 	jal	403b30 <printf@plt>
  401ff4:	02202821 	move	a1,s1
  401ff8:	1000ffd5 	b	401f50 <get_h264_stream_func+0x1d8>
  401ffc:	3c040040 	lui	a0,0x40
  402000:	3c040040 	lui	a0,0x40
  402004:	26a537e8 	addiu	a1,s5,14312
  402008:	268633b0 	addiu	a2,s4,13232
  40200c:	24070190 	li	a3,400
  402010:	0c100ecc 	jal	403b30 <printf@plt>
  402014:	248430f0 	addiu	a0,a0,12528
  402018:	0c100f4c 	jal	403d30 <__errno_location@plt>
  40201c:	00000000 	nop
  402020:	0c100f20 	jal	403c80 <strerror@plt>
  402024:	8c440000 	lw	a0,0(v0)
  402028:	3c040040 	lui	a0,0x40
  40202c:	24843550 	addiu	a0,a0,13648
  402030:	0c100ecc 	jal	403b30 <printf@plt>
  402034:	00402821 	move	a1,v0
  402038:	1000ffc5 	b	401f50 <get_h264_stream_func+0x1d8>
  40203c:	3c040040 	lui	a0,0x40
  402040:	0c100727 	jal	401c9c <video_stop>
  402044:	00000000 	nop
  402048:	0c100f5c 	jal	403d70 <pthread_exit@plt>
  40204c:	00002021 	move	a0,zero

00402050 <video_exit>:
  402050:	27bdffd8 	addiu	sp,sp,-40
  402054:	00002021 	move	a0,zero
  402058:	afbf0024 	sw	ra,36(sp)
  40205c:	afb10020 	sw	s1,32(sp)
  402060:	0c100660 	jal	401980 <encoder_chn_exit>
  402064:	afb0001c 	sw	s0,28(sp)
  402068:	04400032 	bltz	v0,402134 <video_exit+0xe4>
  40206c:	3c040040 	lui	a0,0x40
  402070:	0c100660 	jal	401980 <encoder_chn_exit>
  402074:	24040001 	li	a0,1
  402078:	0440004e 	bltz	v0,4021b4 <video_exit+0x164>
  40207c:	00408021 	move	s0,v0
  402080:	0c100eec 	jal	403bb0 <IMP_Encoder_DestroyGroup@plt>
  402084:	00002021 	move	a0,zero
  402088:	0440003c 	bltz	v0,40217c <video_exit+0x12c>
  40208c:	00408021 	move	s0,v0
  402090:	3c100042 	lui	s0,0x42
  402094:	2610c2e0 	addiu	s0,s0,-15648
  402098:	8e020004 	lw	v0,4(s0)
  40209c:	1440001f 	bnez	v0,40211c <video_exit+0xcc>
  4020a0:	00000000 	nop
  4020a4:	8e02006c 	lw	v0,108(s0)
  4020a8:	14400007 	bnez	v0,4020c8 <video_exit+0x78>
  4020ac:	00000000 	nop
  4020b0:	00001021 	move	v0,zero
  4020b4:	8fbf0024 	lw	ra,36(sp)
  4020b8:	8fb10020 	lw	s1,32(sp)
  4020bc:	8fb0001c 	lw	s0,28(sp)
  4020c0:	03e00008 	jr	ra
  4020c4:	27bd0028 	addiu	sp,sp,40
  4020c8:	0c100f70 	jal	403dc0 <IMP_FrameSource_DestroyChn@plt>
  4020cc:	24040001 	li	a0,1
  4020d0:	0441fff7 	bgez	v0,4020b0 <video_exit+0x60>
  4020d4:	00408821 	move	s1,v0
  4020d8:	3c060040 	lui	a2,0x40
  4020dc:	3c040040 	lui	a0,0x40
  4020e0:	3c050040 	lui	a1,0x40
  4020e4:	24c633b0 	addiu	a2,a2,13232
  4020e8:	24070237 	li	a3,567
  4020ec:	248430f0 	addiu	a0,a0,12528
  4020f0:	0c100ecc 	jal	403b30 <printf@plt>
  4020f4:	24a53834 	addiu	a1,a1,14388
  4020f8:	3c040040 	lui	a0,0x40
  4020fc:	24843608 	addiu	a0,a0,13832
  402100:	0c100ecc 	jal	403b30 <printf@plt>
  402104:	02202821 	move	a1,s1
  402108:	3c040040 	lui	a0,0x40
  40210c:	0c100ecc 	jal	403b30 <printf@plt>
  402110:	2484313c 	addiu	a0,a0,12604
  402114:	1000ffe7 	b	4020b4 <video_exit+0x64>
  402118:	2402ffff 	li	v0,-1
  40211c:	0c100f70 	jal	403dc0 <IMP_FrameSource_DestroyChn@plt>
  402120:	00002021 	move	a0,zero
  402124:	0441ffdf 	bgez	v0,4020a4 <video_exit+0x54>
  402128:	00408821 	move	s1,v0
  40212c:	1000ffeb 	b	4020dc <video_exit+0x8c>
  402130:	3c060040 	lui	a2,0x40
  402134:	3c050040 	lui	a1,0x40
  402138:	3c060040 	lui	a2,0x40
  40213c:	248430f0 	addiu	a0,a0,12528
  402140:	24a53834 	addiu	a1,a1,14388
  402144:	24c633b0 	addiu	a2,a2,13232
  402148:	24070224 	li	a3,548
  40214c:	0c100ecc 	jal	403b30 <printf@plt>
  402150:	00408021 	move	s0,v0
  402154:	3c040040 	lui	a0,0x40
  402158:	248435bc 	addiu	a0,a0,13756
  40215c:	00002821 	move	a1,zero
  402160:	0c100ecc 	jal	403b30 <printf@plt>
  402164:	02003021 	move	a2,s0
  402168:	3c040040 	lui	a0,0x40
  40216c:	0c100ecc 	jal	403b30 <printf@plt>
  402170:	2484313c 	addiu	a0,a0,12604
  402174:	1000ffcf 	b	4020b4 <video_exit+0x64>
  402178:	2402ffff 	li	v0,-1
  40217c:	3c060040 	lui	a2,0x40
  402180:	3c040040 	lui	a0,0x40
  402184:	3c050040 	lui	a1,0x40
  402188:	24c633b0 	addiu	a2,a2,13232
  40218c:	2407022d 	li	a3,557
  402190:	248430f0 	addiu	a0,a0,12528
  402194:	0c100ecc 	jal	403b30 <printf@plt>
  402198:	24a53834 	addiu	a1,a1,14388
  40219c:	3c040040 	lui	a0,0x40
  4021a0:	248435e0 	addiu	a0,a0,13792
  4021a4:	0c100ecc 	jal	403b30 <printf@plt>
  4021a8:	02002821 	move	a1,s0
  4021ac:	1000ffef 	b	40216c <video_exit+0x11c>
  4021b0:	3c040040 	lui	a0,0x40
  4021b4:	3c040040 	lui	a0,0x40
  4021b8:	3c050040 	lui	a1,0x40
  4021bc:	3c060040 	lui	a2,0x40
  4021c0:	240702d2 	li	a3,722
  4021c4:	248430f0 	addiu	a0,a0,12528
  4021c8:	24a53854 	addiu	a1,a1,14420
  4021cc:	0c100ecc 	jal	403b30 <printf@plt>
  4021d0:	24c633b0 	addiu	a2,a2,13232
  4021d4:	3c040040 	lui	a0,0x40
  4021d8:	248435bc 	addiu	a0,a0,13756
  4021dc:	24050001 	li	a1,1
  4021e0:	0c100ecc 	jal	403b30 <printf@plt>
  4021e4:	02003021 	move	a2,s0
  4021e8:	3c040040 	lui	a0,0x40
  4021ec:	0c100ecc 	jal	403b30 <printf@plt>
  4021f0:	2484313c 	addiu	a0,a0,12604
  4021f4:	1000ffa2 	b	402080 <video_exit+0x30>
  4021f8:	00000000 	nop

004021fc <jpeg_init>:
  4021fc:	27bdff18 	addiu	sp,sp,-232
  402200:	3c020042 	lui	v0,0x42
  402204:	afb100d0 	sw	s1,208(sp)
  402208:	3c110042 	lui	s1,0x42
  40220c:	afb400dc 	sw	s4,220(sp)
  402210:	afb200d4 	sw	s2,212(sp)
  402214:	afb000cc 	sw	s0,204(sp)
  402218:	afbf00e4 	sw	ra,228(sp)
  40221c:	afb500e0 	sw	s5,224(sp)
  402220:	afb300d8 	sw	s3,216(sp)
  402224:	2631c2e4 	addiu	s1,s1,-15644
  402228:	00008021 	move	s0,zero
  40222c:	2454c2e0 	addiu	s4,v0,-15648
  402230:	24120002 	li	s2,2
  402234:	8e220000 	lw	v0,0(s1)
  402238:	1440000e 	bnez	v0,402274 <jpeg_init+0x78>
  40223c:	00101100 	sll	v0,s0,0x4
  402240:	26100001 	addiu	s0,s0,1
  402244:	1612fffb 	bne	s0,s2,402234 <jpeg_init+0x38>
  402248:	26310068 	addiu	s1,s1,104
  40224c:	00001021 	move	v0,zero
  402250:	8fbf00e4 	lw	ra,228(sp)
  402254:	8fb500e0 	lw	s5,224(sp)
  402258:	8fb400dc 	lw	s4,220(sp)
  40225c:	8fb300d8 	lw	s3,216(sp)
  402260:	8fb200d4 	lw	s2,212(sp)
  402264:	8fb100d0 	lw	s1,208(sp)
  402268:	8fb000cc 	lw	s0,204(sp)
  40226c:	03e00008 	jr	ra
  402270:	27bd00e8 	addiu	sp,sp,232
  402274:	0010a880 	sll	s5,s0,0x2
  402278:	0055a823 	subu	s5,v0,s5
  40227c:	02b0a821 	addu	s5,s5,s0
  402280:	0015a8c0 	sll	s5,s5,0x3
  402284:	240600ac 	li	a2,172
  402288:	27a40018 	addiu	a0,sp,24
  40228c:	00002821 	move	a1,zero
  402290:	0c100f48 	jal	403d20 <memset@plt>
  402294:	26b30008 	addiu	s3,s5,8
  402298:	02939821 	addu	s3,s4,s3
  40229c:	8e24fffc 	lw	a0,-4(s1)
  4022a0:	8e630000 	lw	v1,0(s3)
  4022a4:	8e620004 	lw	v0,4(s3)
  4022a8:	24840002 	addiu	a0,a0,2
  4022ac:	27a50018 	addiu	a1,sp,24
  4022b0:	afa30024 	sw	v1,36(sp)
  4022b4:	0c100ec4 	jal	403b10 <IMP_Encoder_CreateChn@plt>
  4022b8:	afa20028 	sw	v0,40(sp)
  4022bc:	00409821 	move	s3,v0
  4022c0:	04400019 	bltz	v0,402328 <jpeg_init+0x12c>
  4022c4:	02002021 	move	a0,s0
  4022c8:	8e25fffc 	lw	a1,-4(s1)
  4022cc:	0c100f2c 	jal	403cb0 <IMP_Encoder_RegisterChn@plt>
  4022d0:	24a50002 	addiu	a1,a1,2
  4022d4:	0441ffda 	bgez	v0,402240 <jpeg_init+0x44>
  4022d8:	00409821 	move	s3,v0
  4022dc:	3c040040 	lui	a0,0x40
  4022e0:	3c050040 	lui	a1,0x40
  4022e4:	3c060040 	lui	a2,0x40
  4022e8:	248430f0 	addiu	a0,a0,12528
  4022ec:	24a53860 	addiu	a1,a1,14432
  4022f0:	2407026d 	li	a3,621
  4022f4:	24c633b0 	addiu	a2,a2,13232
  4022f8:	0c100ecc 	jal	403b30 <printf@plt>
  4022fc:	02b4a021 	addu	s4,s5,s4
  402300:	8e850000 	lw	a1,0(s4)
  402304:	3c040040 	lui	a0,0x40
  402308:	24843658 	addiu	a0,a0,13912
  40230c:	0c100ecc 	jal	403b30 <printf@plt>
  402310:	02603021 	move	a2,s3
  402314:	3c040040 	lui	a0,0x40
  402318:	0c100ecc 	jal	403b30 <printf@plt>
  40231c:	2484313c 	addiu	a0,a0,12604
  402320:	1000ffcb 	b	402250 <jpeg_init+0x54>
  402324:	2402ffff 	li	v0,-1
  402328:	3c040040 	lui	a0,0x40
  40232c:	3c050040 	lui	a1,0x40
  402330:	3c060040 	lui	a2,0x40
  402334:	248430f0 	addiu	a0,a0,12528
  402338:	24a53860 	addiu	a1,a1,14432
  40233c:	24070265 	li	a3,613
  402340:	0c100ecc 	jal	403b30 <printf@plt>
  402344:	24c633b0 	addiu	a2,a2,13232
  402348:	02b4a021 	addu	s4,s5,s4
  40234c:	3c040040 	lui	a0,0x40
  402350:	8e850000 	lw	a1,0(s4)
  402354:	1000ffed 	b	40230c <jpeg_init+0x110>
  402358:	24843630 	addiu	a0,a0,13872

0040235c <video_init>:
  40235c:	27bdfef8 	addiu	sp,sp,-264
  402360:	3c020040 	lui	v0,0x40
  402364:	f7b80100 	sdc1	$f24,256(sp)
  402368:	d4583880 	ldc1	$f24,14464(v0)
  40236c:	3c020040 	lui	v0,0x40
  402370:	f7b600f8 	sdc1	$f22,248(sp)
  402374:	d4563888 	ldc1	$f22,14472(v0)
  402378:	3c020040 	lui	v0,0x40
  40237c:	f7b400f0 	sdc1	$f20,240(sp)
  402380:	d4543890 	ldc1	$f20,14480(v0)
  402384:	afb600e4 	sw	s6,228(sp)
  402388:	afb000cc 	sw	s0,204(sp)
  40238c:	3c160042 	lui	s6,0x42
  402390:	3c100042 	lui	s0,0x42
  402394:	afb500e0 	sw	s5,224(sp)
  402398:	afbf00ec 	sw	ra,236(sp)
  40239c:	afb700e8 	sw	s7,232(sp)
  4023a0:	afb400dc 	sw	s4,220(sp)
  4023a4:	afb300d8 	sw	s3,216(sp)
  4023a8:	afb200d4 	sw	s2,212(sp)
  4023ac:	afb100d0 	sw	s1,208(sp)
  4023b0:	26d6c2e4 	addiu	s6,s6,-15644
  4023b4:	0000a821 	move	s5,zero
  4023b8:	2610c2e0 	addiu	s0,s0,-15648
  4023bc:	8ec20000 	lw	v0,0(s6)
  4023c0:	14400018 	bnez	v0,402424 <video_init+0xc8>
  4023c4:	00158880 	sll	s1,s5,0x2
  4023c8:	26b50001 	addiu	s5,s5,1
  4023cc:	24020002 	li	v0,2
  4023d0:	16a2fffa 	bne	s5,v0,4023bc <video_init+0x60>
  4023d4:	26d60068 	addiu	s6,s6,104
  4023d8:	0c10087f 	jal	4021fc <jpeg_init>
  4023dc:	00000000 	nop
  4023e0:	044000b2 	bltz	v0,4026ac <video_init+0x350>
  4023e4:	00001821 	move	v1,zero
  4023e8:	8fbf00ec 	lw	ra,236(sp)
  4023ec:	8fb700e8 	lw	s7,232(sp)
  4023f0:	8fb600e4 	lw	s6,228(sp)
  4023f4:	8fb500e0 	lw	s5,224(sp)
  4023f8:	8fb400dc 	lw	s4,220(sp)
  4023fc:	8fb300d8 	lw	s3,216(sp)
  402400:	8fb200d4 	lw	s2,212(sp)
  402404:	8fb100d0 	lw	s1,208(sp)
  402408:	8fb000cc 	lw	s0,204(sp)
  40240c:	d7b80100 	ldc1	$f24,256(sp)
  402410:	d7b600f8 	ldc1	$f22,248(sp)
  402414:	d7b400f0 	ldc1	$f20,240(sp)
  402418:	00601021 	move	v0,v1
  40241c:	03e00008 	jr	ra
  402420:	27bd0108 	addiu	sp,sp,264
  402424:	00159100 	sll	s2,s5,0x4
  402428:	0251a023 	subu	s4,s2,s1
  40242c:	0295a021 	addu	s4,s4,s5
  402430:	0014a0c0 	sll	s4,s4,0x3
  402434:	8ec4fffc 	lw	a0,-4(s6)
  402438:	26970008 	addiu	s7,s4,8
  40243c:	0217b821 	addu	s7,s0,s7
  402440:	0c100f54 	jal	403d50 <IMP_FrameSource_CreateChn@plt>
  402444:	02e02821 	move	a1,s7
  402448:	02009821 	move	s3,s0
  40244c:	04400064 	bltz	v0,4025e0 <video_init+0x284>
  402450:	02e02821 	move	a1,s7
  402454:	0c100f74 	jal	403dd0 <IMP_FrameSource_SetChnAttr@plt>
  402458:	8ec4fffc 	lw	a0,-4(s6)
  40245c:	04400070 	bltz	v0,402620 <video_init+0x2c4>
  402460:	3c060040 	lui	a2,0x40
  402464:	0c100ee4 	jal	403b90 <IMP_Encoder_CreateGroup@plt>
  402468:	8ec4fffc 	lw	a0,-4(s6)
  40246c:	240600ac 	li	a2,172
  402470:	27a40018 	addiu	a0,sp,24
  402474:	04400076 	bltz	v0,402650 <video_init+0x2f4>
  402478:	00002821 	move	a1,zero
  40247c:	0c100f48 	jal	403d20 <memset@plt>
  402480:	00000000 	nop
  402484:	8ee60000 	lw	a2,0(s7)
  402488:	8ee50004 	lw	a1,4(s7)
  40248c:	8ee4002c 	lw	a0,44(s7)
  402490:	8ee20030 	lw	v0,48(s7)
  402494:	70c54002 	mul	t0,a2,a1
  402498:	00041840 	sll	v1,a0,0x1
  40249c:	afa40048 	sw	a0,72(sp)
  4024a0:	24040003 	li	a0,3
  4024a4:	afa40054 	sw	a0,84(sp)
  4024a8:	2404002d 	li	a0,45
  4024ac:	24070001 	li	a3,1
  4024b0:	afa2004c 	sw	v0,76(sp)
  4024b4:	afa40058 	sw	a0,88(sp)
  4024b8:	2404000f 	li	a0,15
  4024bc:	afa70018 	sw	a3,24(sp)
  4024c0:	afa70020 	sw	a3,32(sp)
  4024c4:	afa60024 	sw	a2,36(sp)
  4024c8:	afa50028 	sw	a1,40(sp)
  4024cc:	afa4005c 	sw	a0,92(sp)
  4024d0:	44881000 	mtc1	t0,$f2
  4024d4:	0062001b 	divu	zero,v1,v0
  4024d8:	004001f4 	teq	v0,zero,0x7
  4024dc:	46801021 	cvt.d.w	$f0,$f2
  4024e0:	46380002 	mul.d	$f0,$f0,$f24
  4024e4:	24030002 	li	v1,2
  4024e8:	afa30060 	sw	v1,96(sp)
  4024ec:	46360003 	div.d	$f0,$f0,$f22
  4024f0:	00001012 	mflo	v0
  4024f4:	4620a03e 	c.le.d	$f20,$f0
  4024f8:	45010033 	bc1t	4025c8 <video_init+0x26c>
  4024fc:	afa20050 	sw	v0,80(sp)
  402500:	4620008d 	trunc.w.d	$f2,$f0
  402504:	44031000 	mfc1	v1,$f2
  402508:	afa30064 	sw	v1,100(sp)
  40250c:	24030050 	li	v1,80
  402510:	2442ffff 	addiu	v0,v0,-1
  402514:	afa3006c 	sw	v1,108(sp)
  402518:	8ec4fffc 	lw	a0,-4(s6)
  40251c:	24030002 	li	v1,2
  402520:	afa30070 	sw	v1,112(sp)
  402524:	afa200ac 	sw	v0,172(sp)
  402528:	24030003 	li	v1,3
  40252c:	24020001 	li	v0,1
  402530:	afa30074 	sw	v1,116(sp)
  402534:	afa200b0 	sw	v0,176(sp)
  402538:	2403000f 	li	v1,15
  40253c:	24020006 	li	v0,6
  402540:	27a50018 	addiu	a1,sp,24
  402544:	afa30078 	sw	v1,120(sp)
  402548:	0c100ec4 	jal	403b10 <IMP_Encoder_CreateChn@plt>
  40254c:	afa200b4 	sw	v0,180(sp)
  402550:	0440004b 	bltz	v0,402680 <video_init+0x324>
  402554:	3c060040 	lui	a2,0x40
  402558:	8ec4fffc 	lw	a0,-4(s6)
  40255c:	0c100f2c 	jal	403cb0 <IMP_Encoder_RegisterChn@plt>
  402560:	00802821 	move	a1,a0
  402564:	0441ff98 	bgez	v0,4023c8 <video_init+0x6c>
  402568:	0040a021 	move	s4,v0
  40256c:	3c040040 	lui	a0,0x40
  402570:	3c050040 	lui	a1,0x40
  402574:	3c060040 	lui	a2,0x40
  402578:	248430f0 	addiu	a0,a0,12528
  40257c:	24a537d0 	addiu	a1,a1,14288
  402580:	24c633b0 	addiu	a2,a2,13232
  402584:	0c100ecc 	jal	403b30 <printf@plt>
  402588:	2407013b 	li	a3,315
  40258c:	02511023 	subu	v0,s2,s1
  402590:	00551021 	addu	v0,v0,s5
  402594:	000210c0 	sll	v0,v0,0x3
  402598:	02629821 	addu	s3,s3,v0
  40259c:	8e650000 	lw	a1,0(s3)
  4025a0:	3c040040 	lui	a0,0x40
  4025a4:	24843728 	addiu	a0,a0,14120
  4025a8:	00a03021 	move	a2,a1
  4025ac:	0c100ecc 	jal	403b30 <printf@plt>
  4025b0:	02803821 	move	a3,s4
  4025b4:	3c040040 	lui	a0,0x40
  4025b8:	0c100ecc 	jal	403b30 <printf@plt>
  4025bc:	2484313c 	addiu	a0,a0,12604
  4025c0:	1000ff89 	b	4023e8 <video_init+0x8c>
  4025c4:	2403ffff 	li	v1,-1
  4025c8:	46340001 	sub.d	$f0,$f0,$f20
  4025cc:	3c048000 	lui	a0,0x8000
  4025d0:	4620008d 	trunc.w.d	$f2,$f0
  4025d4:	44031000 	mfc1	v1,$f2
  4025d8:	1000ffcb 	b	402508 <video_init+0x1ac>
  4025dc:	00641825 	or	v1,v1,a0
  4025e0:	3c060040 	lui	a2,0x40
  4025e4:	3c040040 	lui	a0,0x40
  4025e8:	3c050040 	lui	a1,0x40
  4025ec:	248430f0 	addiu	a0,a0,12528
  4025f0:	24a537d0 	addiu	a1,a1,14288
  4025f4:	24c633b0 	addiu	a2,a2,13232
  4025f8:	240700ca 	li	a3,202
  4025fc:	0c100ecc 	jal	403b30 <printf@plt>
  402600:	02908021 	addu	s0,s4,s0
  402604:	8e050000 	lw	a1,0(s0)
  402608:	3c040040 	lui	a0,0x40
  40260c:	24843684 	addiu	a0,a0,13956
  402610:	0c100ecc 	jal	403b30 <printf@plt>
  402614:	00000000 	nop
  402618:	1000ffe7 	b	4025b8 <video_init+0x25c>
  40261c:	3c040040 	lui	a0,0x40
  402620:	3c040040 	lui	a0,0x40
  402624:	3c050040 	lui	a1,0x40
  402628:	248430f0 	addiu	a0,a0,12528
  40262c:	24a537d0 	addiu	a1,a1,14288
  402630:	24c633b0 	addiu	a2,a2,13232
  402634:	0c100ecc 	jal	403b30 <printf@plt>
  402638:	240700d0 	li	a3,208
  40263c:	02908021 	addu	s0,s4,s0
  402640:	3c040040 	lui	a0,0x40
  402644:	8e050000 	lw	a1,0(s0)
  402648:	1000fff1 	b	402610 <video_init+0x2b4>
  40264c:	248436b0 	addiu	a0,a0,14000
  402650:	3c060040 	lui	a2,0x40
  402654:	3c040040 	lui	a0,0x40
  402658:	3c050040 	lui	a1,0x40
  40265c:	248430f0 	addiu	a0,a0,12528
  402660:	24a537d0 	addiu	a1,a1,14288
  402664:	24c633b0 	addiu	a2,a2,13232
  402668:	0c100ecc 	jal	403b30 <printf@plt>
  40266c:	240700d7 	li	a3,215
  402670:	3c040040 	lui	a0,0x40
  402674:	248436dc 	addiu	a0,a0,14044
  402678:	1000ffe5 	b	402610 <video_init+0x2b4>
  40267c:	02a02821 	move	a1,s5
  402680:	3c040040 	lui	a0,0x40
  402684:	3c050040 	lui	a1,0x40
  402688:	248430f0 	addiu	a0,a0,12528
  40268c:	24a537d0 	addiu	a1,a1,14288
  402690:	24c633b0 	addiu	a2,a2,13232
  402694:	0c100ecc 	jal	403b30 <printf@plt>
  402698:	24070134 	li	a3,308
  40269c:	3c040040 	lui	a0,0x40
  4026a0:	24843704 	addiu	a0,a0,14084
  4026a4:	1000ffda 	b	402610 <video_init+0x2b4>
  4026a8:	02a02821 	move	a1,s5
  4026ac:	3c050040 	lui	a1,0x40
  4026b0:	3c060040 	lui	a2,0x40
  4026b4:	3c040040 	lui	a0,0x40
  4026b8:	24a537d0 	addiu	a1,a1,14288
  4026bc:	24c633b0 	addiu	a2,a2,13232
  4026c0:	24070147 	li	a3,327
  4026c4:	0c100ecc 	jal	403b30 <printf@plt>
  4026c8:	248430f0 	addiu	a0,a0,12528
  4026cc:	3c040040 	lui	a0,0x40
  4026d0:	0c100f1c 	jal	403c70 <puts@plt>
  4026d4:	24843754 	addiu	a0,a0,14164
  4026d8:	3c040040 	lui	a0,0x40
  4026dc:	0c100ecc 	jal	403b30 <printf@plt>
  4026e0:	2484313c 	addiu	a0,a0,12604
  4026e4:	1000ff40 	b	4023e8 <video_init+0x8c>
  4026e8:	2403ffff 	li	v1,-1

004026ec <jpeg_get_one_snap>:
  4026ec:	27bdff78 	addiu	sp,sp,-136
  4026f0:	2c820002 	sltiu	v0,a0,2
  4026f4:	afbf0084 	sw	ra,132(sp)
  4026f8:	afb60080 	sw	s6,128(sp)
  4026fc:	afb5007c 	sw	s5,124(sp)
  402700:	afb40078 	sw	s4,120(sp)
  402704:	afb30074 	sw	s3,116(sp)
  402708:	afb20070 	sw	s2,112(sp)
  40270c:	afb1006c 	sw	s1,108(sp)
  402710:	104000ab 	beqz	v0,4029c0 <jpeg_get_one_snap+0x2d4>
  402714:	afb00068 	sw	s0,104(sp)
  402718:	00808021 	move	s0,a0
  40271c:	00041880 	sll	v1,a0,0x2
  402720:	00042100 	sll	a0,a0,0x4
  402724:	00831823 	subu	v1,a0,v1
  402728:	00701821 	addu	v1,v1,s0
  40272c:	3c020042 	lui	v0,0x42
  402730:	000318c0 	sll	v1,v1,0x3
  402734:	2442c2e0 	addiu	v0,v0,-15648
  402738:	00431021 	addu	v0,v0,v1
  40273c:	8c420004 	lw	v0,4(v0)
  402740:	104000ac 	beqz	v0,4029f4 <jpeg_get_one_snap+0x308>
  402744:	26110002 	addiu	s1,s0,2
  402748:	0c100f88 	jal	403e20 <IMP_Encoder_StartRecvPic@plt>
  40274c:	02202021 	move	a0,s1
  402750:	04400045 	bltz	v0,402868 <jpeg_get_one_snap+0x17c>
  402754:	3c060040 	lui	a2,0x40
  402758:	3c050040 	lui	a1,0x40
  40275c:	27a40018 	addiu	a0,sp,24
  402760:	24a53780 	addiu	a1,a1,14208
  402764:	24c6351c 	addiu	a2,a2,13596
  402768:	02003821 	move	a3,s0
  40276c:	3c160040 	lui	s6,0x40
  402770:	3c130040 	lui	s3,0x40
  402774:	0c100f7c 	jal	403df0 <sprintf@plt>
  402778:	3c120040 	lui	s2,0x40
  40277c:	264633b0 	addiu	a2,s2,13232
  402780:	24070294 	li	a3,660
  402784:	26c430f0 	addiu	a0,s6,12528
  402788:	0c100ecc 	jal	403b30 <printf@plt>
  40278c:	2665386c 	addiu	a1,s3,14444
  402790:	3c040040 	lui	a0,0x40
  402794:	27a50018 	addiu	a1,sp,24
  402798:	24843790 	addiu	a0,a0,14224
  40279c:	0c100ecc 	jal	403b30 <printf@plt>
  4027a0:	3c150040 	lui	s5,0x40
  4027a4:	0c100ecc 	jal	403b30 <printf@plt>
  4027a8:	26a4313c 	addiu	a0,s5,12604
  4027ac:	27a40018 	addiu	a0,sp,24
  4027b0:	24050302 	li	a1,770
  4027b4:	0c100f28 	jal	403ca0 <open@plt>
  4027b8:	240601ff 	li	a2,511
  4027bc:	04400045 	bltz	v0,4028d4 <jpeg_get_one_snap+0x1e8>
  4027c0:	0040a021 	move	s4,v0
  4027c4:	3c040040 	lui	a0,0x40
  4027c8:	2665386c 	addiu	a1,s3,14444
  4027cc:	264633b0 	addiu	a2,s2,13232
  4027d0:	2407029a 	li	a3,666
  4027d4:	0c100ecc 	jal	403b30 <printf@plt>
  4027d8:	24843524 	addiu	a0,a0,13604
  4027dc:	3c040040 	lui	a0,0x40
  4027e0:	0c100f1c 	jal	403c70 <puts@plt>
  4027e4:	2484355c 	addiu	a0,a0,13660
  4027e8:	02202021 	move	a0,s1
  4027ec:	0c100fa4 	jal	403e90 <IMP_Encoder_PollingStream@plt>
  4027f0:	240503e8 	li	a1,1000
  4027f4:	04400055 	bltz	v0,40294c <jpeg_get_one_snap+0x260>
  4027f8:	27a50058 	addiu	a1,sp,88
  4027fc:	02202021 	move	a0,s1
  402800:	0c100f98 	jal	403e60 <IMP_Encoder_GetStream@plt>
  402804:	24060001 	li	a2,1
  402808:	04400066 	bltz	v0,4029a4 <jpeg_get_one_snap+0x2b8>
  40280c:	26c430f0 	addiu	a0,s6,12528
  402810:	02802021 	move	a0,s4
  402814:	0c100620 	jal	401880 <save_stream>
  402818:	27a50058 	addiu	a1,sp,88
  40281c:	04400047 	bltz	v0,40293c <jpeg_get_one_snap+0x250>
  402820:	00408021 	move	s0,v0
  402824:	27a50058 	addiu	a1,sp,88
  402828:	0c100f80 	jal	403e00 <IMP_Encoder_ReleaseStream@plt>
  40282c:	02202021 	move	a0,s1
  402830:	0c100f9c 	jal	403e70 <close@plt>
  402834:	02802021 	move	a0,s4
  402838:	0c100fa0 	jal	403e80 <IMP_Encoder_StopRecvPic@plt>
  40283c:	02202021 	move	a0,s1
  402840:	04410019 	bgez	v0,4028a8 <jpeg_get_one_snap+0x1bc>
  402844:	00008021 	move	s0,zero
  402848:	26c430f0 	addiu	a0,s6,12528
  40284c:	2665386c 	addiu	a1,s3,14444
  402850:	264633b0 	addiu	a2,s2,13232
  402854:	0c100ecc 	jal	403b30 <printf@plt>
  402858:	240702b7 	li	a3,695
  40285c:	3c040040 	lui	a0,0x40
  402860:	10000041 	b	402968 <jpeg_get_one_snap+0x27c>
  402864:	24843598 	addiu	a0,a0,13720
  402868:	3c060040 	lui	a2,0x40
  40286c:	3c040040 	lui	a0,0x40
  402870:	3c050040 	lui	a1,0x40
  402874:	248430f0 	addiu	a0,a0,12528
  402878:	24a5386c 	addiu	a1,a1,14444
  40287c:	24c633b0 	addiu	a2,a2,13232
  402880:	0c100ecc 	jal	403b30 <printf@plt>
  402884:	2407028e 	li	a3,654
  402888:	3c040040 	lui	a0,0x40
  40288c:	248434e0 	addiu	a0,a0,13536
  402890:	0c100ecc 	jal	403b30 <printf@plt>
  402894:	02202821 	move	a1,s1
  402898:	3c040040 	lui	a0,0x40
  40289c:	2484313c 	addiu	a0,a0,12604
  4028a0:	0c100ecc 	jal	403b30 <printf@plt>
  4028a4:	2410ffff 	li	s0,-1
  4028a8:	8fbf0084 	lw	ra,132(sp)
  4028ac:	02001021 	move	v0,s0
  4028b0:	8fb60080 	lw	s6,128(sp)
  4028b4:	8fb5007c 	lw	s5,124(sp)
  4028b8:	8fb40078 	lw	s4,120(sp)
  4028bc:	8fb30074 	lw	s3,116(sp)
  4028c0:	8fb20070 	lw	s2,112(sp)
  4028c4:	8fb1006c 	lw	s1,108(sp)
  4028c8:	8fb00068 	lw	s0,104(sp)
  4028cc:	03e00008 	jr	ra
  4028d0:	27bd0088 	addiu	sp,sp,136
  4028d4:	264633b0 	addiu	a2,s2,13232
  4028d8:	24070297 	li	a3,663
  4028dc:	2665386c 	addiu	a1,s3,14444
  4028e0:	0c100ecc 	jal	403b30 <printf@plt>
  4028e4:	26c430f0 	addiu	a0,s6,12528
  4028e8:	0c100f4c 	jal	403d30 <__errno_location@plt>
  4028ec:	2410ffff 	li	s0,-1
  4028f0:	0c100f20 	jal	403c80 <strerror@plt>
  4028f4:	8c440000 	lw	a0,0(v0)
  4028f8:	3c040040 	lui	a0,0x40
  4028fc:	00402821 	move	a1,v0
  402900:	0c100ecc 	jal	403b30 <printf@plt>
  402904:	24843550 	addiu	a0,a0,13648
  402908:	0c100ecc 	jal	403b30 <printf@plt>
  40290c:	26a4313c 	addiu	a0,s5,12604
  402910:	8fbf0084 	lw	ra,132(sp)
  402914:	02001021 	move	v0,s0
  402918:	8fb60080 	lw	s6,128(sp)
  40291c:	8fb5007c 	lw	s5,124(sp)
  402920:	8fb40078 	lw	s4,120(sp)
  402924:	8fb30074 	lw	s3,116(sp)
  402928:	8fb20070 	lw	s2,112(sp)
  40292c:	8fb1006c 	lw	s1,108(sp)
  402930:	8fb00068 	lw	s0,104(sp)
  402934:	03e00008 	jr	ra
  402938:	27bd0088 	addiu	sp,sp,136
  40293c:	0c100f9c 	jal	403e70 <close@plt>
  402940:	02802021 	move	a0,s4
  402944:	1000ffd9 	b	4028ac <jpeg_get_one_snap+0x1c0>
  402948:	8fbf0084 	lw	ra,132(sp)
  40294c:	26c430f0 	addiu	a0,s6,12528
  402950:	2665386c 	addiu	a1,s3,14444
  402954:	264633b0 	addiu	a2,s2,13232
  402958:	0c100ecc 	jal	403b30 <printf@plt>
  40295c:	2407029f 	li	a3,671
  402960:	3c040040 	lui	a0,0x40
  402964:	24843560 	addiu	a0,a0,13664
  402968:	0c100f1c 	jal	403c70 <puts@plt>
  40296c:	2410ffff 	li	s0,-1
  402970:	0c100ecc 	jal	403b30 <printf@plt>
  402974:	26a4313c 	addiu	a0,s5,12604
  402978:	8fbf0084 	lw	ra,132(sp)
  40297c:	02001021 	move	v0,s0
  402980:	8fb60080 	lw	s6,128(sp)
  402984:	8fb5007c 	lw	s5,124(sp)
  402988:	8fb40078 	lw	s4,120(sp)
  40298c:	8fb30074 	lw	s3,116(sp)
  402990:	8fb20070 	lw	s2,112(sp)
  402994:	8fb1006c 	lw	s1,108(sp)
  402998:	8fb00068 	lw	s0,104(sp)
  40299c:	03e00008 	jr	ra
  4029a0:	27bd0088 	addiu	sp,sp,136
  4029a4:	2665386c 	addiu	a1,s3,14444
  4029a8:	264633b0 	addiu	a2,s2,13232
  4029ac:	0c100ecc 	jal	403b30 <printf@plt>
  4029b0:	240702a7 	li	a3,679
  4029b4:	3c040040 	lui	a0,0x40
  4029b8:	1000ffeb 	b	402968 <jpeg_get_one_snap+0x27c>
  4029bc:	24843578 	addiu	a0,a0,13688
  4029c0:	3c050040 	lui	a1,0x40
  4029c4:	3c060040 	lui	a2,0x40
  4029c8:	3c040040 	lui	a0,0x40
  4029cc:	24a5386c 	addiu	a1,a1,14444
  4029d0:	24c633b0 	addiu	a2,a2,13232
  4029d4:	24070286 	li	a3,646
  4029d8:	0c100ecc 	jal	403b30 <printf@plt>
  4029dc:	248430f0 	addiu	a0,a0,12528
  4029e0:	3c040040 	lui	a0,0x40
  4029e4:	0c100f1c 	jal	403c70 <puts@plt>
  4029e8:	24843768 	addiu	a0,a0,14184
  4029ec:	1000ffab 	b	40289c <jpeg_get_one_snap+0x1b0>
  4029f0:	3c040040 	lui	a0,0x40
  4029f4:	3c060040 	lui	a2,0x40
  4029f8:	3c040040 	lui	a0,0x40
  4029fc:	3c050040 	lui	a1,0x40
  402a00:	24c633b0 	addiu	a2,a2,13232
  402a04:	240702bc 	li	a3,700
  402a08:	248430f0 	addiu	a0,a0,12528
  402a0c:	0c100ecc 	jal	403b30 <printf@plt>
  402a10:	24a5386c 	addiu	a1,a1,14444
  402a14:	3c040040 	lui	a0,0x40
  402a18:	02002821 	move	a1,s0
  402a1c:	0c100ecc 	jal	403b30 <printf@plt>
  402a20:	248437a4 	addiu	a0,a0,14244
  402a24:	3c040040 	lui	a0,0x40
  402a28:	2484313c 	addiu	a0,a0,12604
  402a2c:	0c100ecc 	jal	403b30 <printf@plt>
  402a30:	2410ffff 	li	s0,-1
  402a34:	1000ff9d 	b	4028ac <jpeg_get_one_snap+0x1c0>
  402a38:	8fbf0084 	lw	ra,132(sp)
  402a3c:	00000000 	nop

00402a40 <T20_system_init>:
  402a40:	27bdffd0 	addiu	sp,sp,-48
  402a44:	afb1001c 	sw	s1,28(sp)
  402a48:	3c110042 	lui	s1,0x42
  402a4c:	afb00018 	sw	s0,24(sp)
  402a50:	2630ce90 	addiu	s0,s1,-12656
  402a54:	02002021 	move	a0,s0
  402a58:	00002821 	move	a1,zero
  402a5c:	24060050 	li	a2,80
  402a60:	afbf002c 	sw	ra,44(sp)
  402a64:	afb40028 	sw	s4,40(sp)
  402a68:	afb30024 	sw	s3,36(sp)
  402a6c:	0c100f48 	jal	403d20 <memset@plt>
  402a70:	afb20020 	sw	s2,32(sp)
  402a74:	3c030042 	lui	v1,0x42
  402a78:	2469ceb4 	addiu	t1,v1,-12620
  402a7c:	24020032 	li	v0,50
  402a80:	3c140040 	lui	s4,0x40
  402a84:	3c130040 	lui	s3,0x40
  402a88:	3c120040 	lui	s2,0x40
  402a8c:	3c083266 	lui	t0,0x3266
  402a90:	2508786a 	addiu	t0,t0,30826
  402a94:	240a0001 	li	t2,1
  402a98:	a5220004 	sh	v0,4(t1)
  402a9c:	26653a70 	addiu	a1,s3,14960
  402aa0:	26463898 	addiu	a2,s2,14488
  402aa4:	2407002a 	li	a3,42
  402aa8:	26843524 	addiu	a0,s4,13604
  402aac:	a6020004 	sh	v0,4(s0)
  402ab0:	24020040 	li	v0,64
  402ab4:	ae28ce90 	sw	t0,-12656(s1)
  402ab8:	ae0a0020 	sw	t2,32(s0)
  402abc:	ac68ceb4 	sw	t0,-12620(v1)
  402ac0:	0c100ecc 	jal	403b30 <printf@plt>
  402ac4:	ae020038 	sw	v0,56(s0)
  402ac8:	3c040040 	lui	a0,0x40
  402acc:	0c100f1c 	jal	403c70 <puts@plt>
  402ad0:	248438a4 	addiu	a0,a0,14500
  402ad4:	0c100f40 	jal	403d00 <IMP_ISP_Open@plt>
  402ad8:	00000000 	nop
  402adc:	04400026 	bltz	v0,402b78 <T20_system_init+0x138>
  402ae0:	3c040040 	lui	a0,0x40
  402ae4:	0c100f78 	jal	403de0 <IMP_ISP_AddSensor@plt>
  402ae8:	02002021 	move	a0,s0
  402aec:	04400030 	bltz	v0,402bb0 <T20_system_init+0x170>
  402af0:	3c040040 	lui	a0,0x40
  402af4:	0c100ebc 	jal	403af0 <IMP_ISP_EnableSensor@plt>
  402af8:	00000000 	nop
  402afc:	04400034 	bltz	v0,402bd0 <T20_system_init+0x190>
  402b00:	3c040040 	lui	a0,0x40
  402b04:	0c100f08 	jal	403c20 <IMP_System_Init@plt>
  402b08:	00000000 	nop
  402b0c:	04400038 	bltz	v0,402bf0 <T20_system_init+0x1b0>
  402b10:	3c040040 	lui	a0,0x40
  402b14:	0c100f00 	jal	403c00 <IMP_ISP_EnableTuning@plt>
  402b18:	00000000 	nop
  402b1c:	0440003c 	bltz	v0,402c10 <T20_system_init+0x1d0>
  402b20:	3c040040 	lui	a0,0x40
  402b24:	2404001e 	li	a0,30
  402b28:	0c100f24 	jal	403c90 <IMP_ISP_Tuning_SetSensorFPS@plt>
  402b2c:	24050001 	li	a1,1
  402b30:	0440003f 	bltz	v0,402c30 <T20_system_init+0x1f0>
  402b34:	26653a70 	addiu	a1,s3,14960
  402b38:	26843524 	addiu	a0,s4,13604
  402b3c:	26463898 	addiu	a2,s2,14488
  402b40:	0c100ecc 	jal	403b30 <printf@plt>
  402b44:	24070051 	li	a3,81
  402b48:	3c040040 	lui	a0,0x40
  402b4c:	0c100f1c 	jal	403c70 <puts@plt>
  402b50:	24843950 	addiu	a0,a0,14672
  402b54:	00001021 	move	v0,zero
  402b58:	8fbf002c 	lw	ra,44(sp)
  402b5c:	8fb40028 	lw	s4,40(sp)
  402b60:	8fb30024 	lw	s3,36(sp)
  402b64:	8fb20020 	lw	s2,32(sp)
  402b68:	8fb1001c 	lw	s1,28(sp)
  402b6c:	8fb00018 	lw	s0,24(sp)
  402b70:	03e00008 	jr	ra
  402b74:	27bd0030 	addiu	sp,sp,48
  402b78:	248430f0 	addiu	a0,a0,12528
  402b7c:	26653a70 	addiu	a1,s3,14960
  402b80:	26463898 	addiu	a2,s2,14488
  402b84:	0c100ecc 	jal	403b30 <printf@plt>
  402b88:	2407002e 	li	a3,46
  402b8c:	3c040040 	lui	a0,0x40
  402b90:	248438c0 	addiu	a0,a0,14528
  402b94:	0c100f1c 	jal	403c70 <puts@plt>
  402b98:	00000000 	nop
  402b9c:	3c040040 	lui	a0,0x40
  402ba0:	0c100ecc 	jal	403b30 <printf@plt>
  402ba4:	2484313c 	addiu	a0,a0,12604
  402ba8:	1000ffeb 	b	402b58 <T20_system_init+0x118>
  402bac:	2402ffff 	li	v0,-1
  402bb0:	248430f0 	addiu	a0,a0,12528
  402bb4:	26653a70 	addiu	a1,s3,14960
  402bb8:	26463898 	addiu	a2,s2,14488
  402bbc:	0c100ecc 	jal	403b30 <printf@plt>
  402bc0:	24070034 	li	a3,52
  402bc4:	3c040040 	lui	a0,0x40
  402bc8:	1000fff2 	b	402b94 <T20_system_init+0x154>
  402bcc:	248438d4 	addiu	a0,a0,14548
  402bd0:	248430f0 	addiu	a0,a0,12528
  402bd4:	26653a70 	addiu	a1,s3,14960
  402bd8:	26463898 	addiu	a2,s2,14488
  402bdc:	0c100ecc 	jal	403b30 <printf@plt>
  402be0:	2407003a 	li	a3,58
  402be4:	3c040040 	lui	a0,0x40
  402be8:	1000ffea 	b	402b94 <T20_system_init+0x154>
  402bec:	248438e8 	addiu	a0,a0,14568
  402bf0:	248430f0 	addiu	a0,a0,12528
  402bf4:	26653a70 	addiu	a1,s3,14960
  402bf8:	26463898 	addiu	a2,s2,14488
  402bfc:	0c100ecc 	jal	403b30 <printf@plt>
  402c00:	24070040 	li	a3,64
  402c04:	3c040040 	lui	a0,0x40
  402c08:	1000ffe2 	b	402b94 <T20_system_init+0x154>
  402c0c:	24843900 	addiu	a0,a0,14592
  402c10:	248430f0 	addiu	a0,a0,12528
  402c14:	26653a70 	addiu	a1,s3,14960
  402c18:	26463898 	addiu	a2,s2,14488
  402c1c:	0c100ecc 	jal	403b30 <printf@plt>
  402c20:	24070047 	li	a3,71
  402c24:	3c040040 	lui	a0,0x40
  402c28:	1000ffda 	b	402b94 <T20_system_init+0x154>
  402c2c:	24843918 	addiu	a0,a0,14616
  402c30:	3c040040 	lui	a0,0x40
  402c34:	248430f0 	addiu	a0,a0,12528
  402c38:	26463898 	addiu	a2,s2,14488
  402c3c:	0c100ecc 	jal	403b30 <printf@plt>
  402c40:	2407004d 	li	a3,77
  402c44:	3c040040 	lui	a0,0x40
  402c48:	1000ffd2 	b	402b94 <T20_system_init+0x154>
  402c4c:	24843934 	addiu	a0,a0,14644

00402c50 <T20_system_exit>:
  402c50:	27bdffd8 	addiu	sp,sp,-40
  402c54:	afb20020 	sw	s2,32(sp)
  402c58:	afb1001c 	sw	s1,28(sp)
  402c5c:	afb00018 	sw	s0,24(sp)
  402c60:	3c120040 	lui	s2,0x40
  402c64:	3c110040 	lui	s1,0x40
  402c68:	3c100040 	lui	s0,0x40
  402c6c:	26253a80 	addiu	a1,s1,14976
  402c70:	26063898 	addiu	a2,s0,14488
  402c74:	24070061 	li	a3,97
  402c78:	afbf0024 	sw	ra,36(sp)
  402c7c:	0c100ecc 	jal	403b30 <printf@plt>
  402c80:	26443524 	addiu	a0,s2,13604
  402c84:	3c040040 	lui	a0,0x40
  402c88:	0c100f1c 	jal	403c70 <puts@plt>
  402c8c:	24843968 	addiu	a0,a0,14696
  402c90:	0c100ef4 	jal	403bd0 <IMP_System_Exit@plt>
  402c94:	00000000 	nop
  402c98:	0c100f58 	jal	403d60 <IMP_ISP_DisableSensor@plt>
  402c9c:	00000000 	nop
  402ca0:	0440002a 	bltz	v0,402d4c <T20_system_exit+0xfc>
  402ca4:	26253a80 	addiu	a1,s1,14976
  402ca8:	3c040042 	lui	a0,0x42
  402cac:	0c100ef8 	jal	403be0 <IMP_ISP_DelSensor@plt>
  402cb0:	2484ce90 	addiu	a0,a0,-12656
  402cb4:	0440002d 	bltz	v0,402d6c <T20_system_exit+0x11c>
  402cb8:	3c040040 	lui	a0,0x40
  402cbc:	0c100eb8 	jal	403ae0 <IMP_ISP_DisableTuning@plt>
  402cc0:	00000000 	nop
  402cc4:	04400031 	bltz	v0,402d8c <T20_system_exit+0x13c>
  402cc8:	3c040040 	lui	a0,0x40
  402ccc:	0c100f04 	jal	403c10 <IMP_ISP_Close@plt>
  402cd0:	00000000 	nop
  402cd4:	1440000f 	bnez	v0,402d14 <T20_system_exit+0xc4>
  402cd8:	26253a80 	addiu	a1,s1,14976
  402cdc:	26443524 	addiu	a0,s2,13604
  402ce0:	26063898 	addiu	a2,s0,14488
  402ce4:	0c100ecc 	jal	403b30 <printf@plt>
  402ce8:	2407007c 	li	a3,124
  402cec:	3c040040 	lui	a0,0x40
  402cf0:	0c100f1c 	jal	403c70 <puts@plt>
  402cf4:	248439a4 	addiu	a0,a0,14756
  402cf8:	00001021 	move	v0,zero
  402cfc:	8fbf0024 	lw	ra,36(sp)
  402d00:	8fb20020 	lw	s2,32(sp)
  402d04:	8fb1001c 	lw	s1,28(sp)
  402d08:	8fb00018 	lw	s0,24(sp)
  402d0c:	03e00008 	jr	ra
  402d10:	27bd0028 	addiu	sp,sp,40
  402d14:	3c040040 	lui	a0,0x40
  402d18:	248430f0 	addiu	a0,a0,12528
  402d1c:	26063898 	addiu	a2,s0,14488
  402d20:	0c100ecc 	jal	403b30 <printf@plt>
  402d24:	24070078 	li	a3,120
  402d28:	3c040040 	lui	a0,0x40
  402d2c:	248438c0 	addiu	a0,a0,14528
  402d30:	0c100f1c 	jal	403c70 <puts@plt>
  402d34:	00000000 	nop
  402d38:	3c040040 	lui	a0,0x40
  402d3c:	0c100ecc 	jal	403b30 <printf@plt>
  402d40:	2484313c 	addiu	a0,a0,12604
  402d44:	1000ffed 	b	402cfc <T20_system_exit+0xac>
  402d48:	2402ffff 	li	v0,-1
  402d4c:	3c040040 	lui	a0,0x40
  402d50:	248430f0 	addiu	a0,a0,12528
  402d54:	26063898 	addiu	a2,s0,14488
  402d58:	0c100ecc 	jal	403b30 <printf@plt>
  402d5c:	24070067 	li	a3,103
  402d60:	3c040040 	lui	a0,0x40
  402d64:	1000fff2 	b	402d30 <T20_system_exit+0xe0>
  402d68:	248438e8 	addiu	a0,a0,14568
  402d6c:	248430f0 	addiu	a0,a0,12528
  402d70:	26253a80 	addiu	a1,s1,14976
  402d74:	26063898 	addiu	a2,s0,14488
  402d78:	0c100ecc 	jal	403b30 <printf@plt>
  402d7c:	2407006d 	li	a3,109
  402d80:	3c040040 	lui	a0,0x40
  402d84:	1000ffea 	b	402d30 <T20_system_exit+0xe0>
  402d88:	248438d4 	addiu	a0,a0,14548
  402d8c:	248430f0 	addiu	a0,a0,12528
  402d90:	26253a80 	addiu	a1,s1,14976
  402d94:	26063898 	addiu	a2,s0,14488
  402d98:	0c100ecc 	jal	403b30 <printf@plt>
  402d9c:	24070073 	li	a3,115
  402da0:	3c040040 	lui	a0,0x40
  402da4:	1000ffe2 	b	402d30 <T20_system_exit+0xe0>
  402da8:	24843984 	addiu	a0,a0,14724

00402dac <encoder_system_init>:
  402dac:	27bdffc8 	addiu	sp,sp,-56
  402db0:	afbf0034 	sw	ra,52(sp)
  402db4:	afb50030 	sw	s5,48(sp)
  402db8:	afb4002c 	sw	s4,44(sp)
  402dbc:	afb30028 	sw	s3,40(sp)
  402dc0:	afb20024 	sw	s2,36(sp)
  402dc4:	afb10020 	sw	s1,32(sp)
  402dc8:	0c100a90 	jal	402a40 <T20_system_init>
  402dcc:	afb0001c 	sw	s0,28(sp)
  402dd0:	04400054 	bltz	v0,402f24 <encoder_system_init+0x178>
  402dd4:	3c050040 	lui	a1,0x40
  402dd8:	0c1008d7 	jal	40235c <video_init>
  402ddc:	00000000 	nop
  402de0:	0440005d 	bltz	v0,402f58 <encoder_system_init+0x1ac>
  402de4:	3c050040 	lui	a1,0x40
  402de8:	0c100518 	jal	401460 <osd_init>
  402dec:	00000000 	nop
  402df0:	0440005e 	bltz	v0,402f6c <encoder_system_init+0x1c0>
  402df4:	3c120042 	lui	s2,0x42
  402df8:	2652c2e0 	addiu	s2,s2,-15648
  402dfc:	00008021 	move	s0,zero
  402e00:	0240a021 	move	s4,s2
  402e04:	24150002 	li	s5,2
  402e08:	00101100 	sll	v0,s0,0x4
  402e0c:	00108880 	sll	s1,s0,0x2
  402e10:	00518823 	subu	s1,v0,s1
  402e14:	02308821 	addu	s1,s1,s0
  402e18:	001188c0 	sll	s1,s1,0x3
  402e1c:	02519821 	addu	s3,s2,s1
  402e20:	2673005c 	addiu	s3,s3,92
  402e24:	02512021 	addu	a0,s2,s1
  402e28:	24840044 	addiu	a0,a0,68
  402e2c:	0c100ec8 	jal	403b20 <IMP_System_Bind@plt>
  402e30:	02602821 	move	a1,s3
  402e34:	26250050 	addiu	a1,s1,80
  402e38:	02602021 	move	a0,s3
  402e3c:	04400011 	bltz	v0,402e84 <encoder_system_init+0xd8>
  402e40:	02852821 	addu	a1,s4,a1
  402e44:	0c100ec8 	jal	403b20 <IMP_System_Bind@plt>
  402e48:	00000000 	nop
  402e4c:	04400027 	bltz	v0,402eec <encoder_system_init+0x140>
  402e50:	3c040040 	lui	a0,0x40
  402e54:	26100001 	addiu	s0,s0,1
  402e58:	1615ffeb 	bne	s0,s5,402e08 <encoder_system_init+0x5c>
  402e5c:	8fbf0034 	lw	ra,52(sp)
  402e60:	8fb50030 	lw	s5,48(sp)
  402e64:	8fb4002c 	lw	s4,44(sp)
  402e68:	8fb30028 	lw	s3,40(sp)
  402e6c:	8fb20024 	lw	s2,36(sp)
  402e70:	8fb10020 	lw	s1,32(sp)
  402e74:	8fb0001c 	lw	s0,28(sp)
  402e78:	00001021 	move	v0,zero
  402e7c:	03e00008 	jr	ra
  402e80:	27bd0038 	addiu	sp,sp,56
  402e84:	3c040040 	lui	a0,0x40
  402e88:	3c050040 	lui	a1,0x40
  402e8c:	3c060040 	lui	a2,0x40
  402e90:	248430f0 	addiu	a0,a0,12528
  402e94:	24a53a90 	addiu	a1,a1,14992
  402e98:	24c63898 	addiu	a2,a2,14488
  402e9c:	0c100ecc 	jal	403b30 <printf@plt>
  402ea0:	240700a7 	li	a3,167
  402ea4:	3c040040 	lui	a0,0x40
  402ea8:	02002821 	move	a1,s0
  402eac:	248439d0 	addiu	a0,a0,14800
  402eb0:	0c100ecc 	jal	403b30 <printf@plt>
  402eb4:	02003021 	move	a2,s0
  402eb8:	3c040040 	lui	a0,0x40
  402ebc:	0c100ecc 	jal	403b30 <printf@plt>
  402ec0:	2484313c 	addiu	a0,a0,12604
  402ec4:	8fbf0034 	lw	ra,52(sp)
  402ec8:	8fb50030 	lw	s5,48(sp)
  402ecc:	8fb4002c 	lw	s4,44(sp)
  402ed0:	8fb30028 	lw	s3,40(sp)
  402ed4:	8fb20024 	lw	s2,36(sp)
  402ed8:	8fb10020 	lw	s1,32(sp)
  402edc:	8fb0001c 	lw	s0,28(sp)
  402ee0:	2402ffff 	li	v0,-1
  402ee4:	03e00008 	jr	ra
  402ee8:	27bd0038 	addiu	sp,sp,56
  402eec:	3c050040 	lui	a1,0x40
  402ef0:	3c060040 	lui	a2,0x40
  402ef4:	240700ad 	li	a3,173
  402ef8:	248430f0 	addiu	a0,a0,12528
  402efc:	24a53a90 	addiu	a1,a1,14992
  402f00:	0c100ecc 	jal	403b30 <printf@plt>
  402f04:	24c63898 	addiu	a2,a2,14488
  402f08:	3c040040 	lui	a0,0x40
  402f0c:	02002821 	move	a1,s0
  402f10:	24843a04 	addiu	a0,a0,14852
  402f14:	0c100ecc 	jal	403b30 <printf@plt>
  402f18:	02003021 	move	a2,s0
  402f1c:	1000ffe7 	b	402ebc <encoder_system_init+0x110>
  402f20:	3c040040 	lui	a0,0x40
  402f24:	3c060040 	lui	a2,0x40
  402f28:	24a53a90 	addiu	a1,a1,14992
  402f2c:	24c63898 	addiu	a2,a2,14488
  402f30:	24070091 	li	a3,145
  402f34:	3c040040 	lui	a0,0x40
  402f38:	0c100ecc 	jal	403b30 <printf@plt>
  402f3c:	248430f0 	addiu	a0,a0,12528
  402f40:	3c040040 	lui	a0,0x40
  402f44:	24843754 	addiu	a0,a0,14164
  402f48:	0c100f1c 	jal	403c70 <puts@plt>
  402f4c:	00000000 	nop
  402f50:	1000ffda 	b	402ebc <encoder_system_init+0x110>
  402f54:	3c040040 	lui	a0,0x40
  402f58:	3c060040 	lui	a2,0x40
  402f5c:	24a53a90 	addiu	a1,a1,14992
  402f60:	24c63898 	addiu	a2,a2,14488
  402f64:	1000fff3 	b	402f34 <encoder_system_init+0x188>
  402f68:	24070097 	li	a3,151
  402f6c:	3c050040 	lui	a1,0x40
  402f70:	3c060040 	lui	a2,0x40
  402f74:	3c040040 	lui	a0,0x40
  402f78:	248430f0 	addiu	a0,a0,12528
  402f7c:	24a53a90 	addiu	a1,a1,14992
  402f80:	24c63898 	addiu	a2,a2,14488
  402f84:	0c100ecc 	jal	403b30 <printf@plt>
  402f88:	2407009e 	li	a3,158
  402f8c:	3c040040 	lui	a0,0x40
  402f90:	1000ffed 	b	402f48 <encoder_system_init+0x19c>
  402f94:	248439c0 	addiu	a0,a0,14784

00402f98 <encoder_system_exit>:
  402f98:	27bdffc8 	addiu	sp,sp,-56
  402f9c:	afb20024 	sw	s2,36(sp)
  402fa0:	3c120042 	lui	s2,0x42
  402fa4:	2652c2e0 	addiu	s2,s2,-15648
  402fa8:	afb50030 	sw	s5,48(sp)
  402fac:	afb4002c 	sw	s4,44(sp)
  402fb0:	afb0001c 	sw	s0,28(sp)
  402fb4:	afbf0034 	sw	ra,52(sp)
  402fb8:	afb30028 	sw	s3,40(sp)
  402fbc:	afb10020 	sw	s1,32(sp)
  402fc0:	00008021 	move	s0,zero
  402fc4:	0240a021 	move	s4,s2
  402fc8:	24150002 	li	s5,2
  402fcc:	00101080 	sll	v0,s0,0x2
  402fd0:	00108900 	sll	s1,s0,0x4
  402fd4:	02228823 	subu	s1,s1,v0
  402fd8:	02308821 	addu	s1,s1,s0
  402fdc:	001188c0 	sll	s1,s1,0x3
  402fe0:	02519821 	addu	s3,s2,s1
  402fe4:	2673005c 	addiu	s3,s3,92
  402fe8:	26250050 	addiu	a1,s1,80
  402fec:	02602021 	move	a0,s3
  402ff0:	0c100f84 	jal	403e10 <IMP_System_UnBind@plt>
  402ff4:	02452821 	addu	a1,s2,a1
  402ff8:	02912021 	addu	a0,s4,s1
  402ffc:	24840044 	addiu	a0,a0,68
  403000:	02602821 	move	a1,s3
  403004:	04400017 	bltz	v0,403064 <encoder_system_exit+0xcc>
  403008:	26100001 	addiu	s0,s0,1
  40300c:	0c100f84 	jal	403e10 <IMP_System_UnBind@plt>
  403010:	00000000 	nop
  403014:	0440002c 	bltz	v0,4030c8 <encoder_system_exit+0x130>
  403018:	3c050040 	lui	a1,0x40
  40301c:	1615ffec 	bne	s0,s5,402fd0 <encoder_system_exit+0x38>
  403020:	00101080 	sll	v0,s0,0x2
  403024:	0c1005a3 	jal	40168c <osd_exit>
  403028:	00000000 	nop
  40302c:	0c100814 	jal	402050 <video_exit>
  403030:	00000000 	nop
  403034:	0c100b14 	jal	402c50 <T20_system_exit>
  403038:	00000000 	nop
  40303c:	8fbf0034 	lw	ra,52(sp)
  403040:	8fb50030 	lw	s5,48(sp)
  403044:	8fb4002c 	lw	s4,44(sp)
  403048:	8fb30028 	lw	s3,40(sp)
  40304c:	8fb20024 	lw	s2,36(sp)
  403050:	8fb10020 	lw	s1,32(sp)
  403054:	8fb0001c 	lw	s0,28(sp)
  403058:	00001021 	move	v0,zero
  40305c:	03e00008 	jr	ra
  403060:	27bd0038 	addiu	sp,sp,56
  403064:	3c050040 	lui	a1,0x40
  403068:	3c060040 	lui	a2,0x40
  40306c:	3c040040 	lui	a0,0x40
  403070:	248430f0 	addiu	a0,a0,12528
  403074:	24a53aa4 	addiu	a1,a1,15012
  403078:	24c63898 	addiu	a2,a2,14488
  40307c:	0c100ecc 	jal	403b30 <printf@plt>
  403080:	240700c8 	li	a3,200
  403084:	3c040040 	lui	a0,0x40
  403088:	24843a2c 	addiu	a0,a0,14892
  40308c:	0c100f1c 	jal	403c70 <puts@plt>
  403090:	00000000 	nop
  403094:	3c040040 	lui	a0,0x40
  403098:	0c100ecc 	jal	403b30 <printf@plt>
  40309c:	2484313c 	addiu	a0,a0,12604
  4030a0:	8fbf0034 	lw	ra,52(sp)
  4030a4:	8fb50030 	lw	s5,48(sp)
  4030a8:	8fb4002c 	lw	s4,44(sp)
  4030ac:	8fb30028 	lw	s3,40(sp)
  4030b0:	8fb20024 	lw	s2,36(sp)
  4030b4:	8fb10020 	lw	s1,32(sp)
  4030b8:	8fb0001c 	lw	s0,28(sp)
  4030bc:	2402ffff 	li	v0,-1
  4030c0:	03e00008 	jr	ra
  4030c4:	27bd0038 	addiu	sp,sp,56
  4030c8:	3c060040 	lui	a2,0x40
  4030cc:	3c040040 	lui	a0,0x40
  4030d0:	248430f0 	addiu	a0,a0,12528
  4030d4:	24a53aa4 	addiu	a1,a1,15012
  4030d8:	24c63898 	addiu	a2,a2,14488
  4030dc:	0c100ecc 	jal	403b30 <printf@plt>
  4030e0:	240700ce 	li	a3,206
  4030e4:	3c040040 	lui	a0,0x40
  4030e8:	1000ffe8 	b	40308c <encoder_system_exit+0xf4>
  4030ec:	24843a4c 	addiu	a0,a0,14924

Disassembly of section .plt:

00403ac0 <_PROCEDURE_LINKAGE_TABLE_>:
  403ac0:	3c1c0041 	lui	gp,0x41
  403ac4:	8f994000 	lw	t9,16384(gp)
  403ac8:	279c4000 	addiu	gp,gp,16384
  403acc:	031cc023 	subu	t8,t8,gp
  403ad0:	03e07821 	move	t7,ra
  403ad4:	0018c082 	srl	t8,t8,0x2
  403ad8:	0320f809 	jalr	t9
  403adc:	2718fffe 	addiu	t8,t8,-2

00403ae0 <IMP_ISP_DisableTuning@plt>:
  403ae0:	3c0f0041 	lui	t7,0x41
  403ae4:	8df94008 	lw	t9,16392(t7)
  403ae8:	03200008 	jr	t9
  403aec:	25f84008 	addiu	t8,t7,16392

00403af0 <IMP_ISP_EnableSensor@plt>:
  403af0:	3c0f0041 	lui	t7,0x41
  403af4:	8df9400c 	lw	t9,16396(t7)
  403af8:	03200008 	jr	t9
  403afc:	25f8400c 	addiu	t8,t7,16396

00403b00 <IMP_OSD_UnRegisterRgn@plt>:
  403b00:	3c0f0041 	lui	t7,0x41
  403b04:	8df94010 	lw	t9,16400(t7)
  403b08:	03200008 	jr	t9
  403b0c:	25f84010 	addiu	t8,t7,16400

00403b10 <IMP_Encoder_CreateChn@plt>:
  403b10:	3c0f0041 	lui	t7,0x41
  403b14:	8df94014 	lw	t9,16404(t7)
  403b18:	03200008 	jr	t9
  403b1c:	25f84014 	addiu	t8,t7,16404

00403b20 <IMP_System_Bind@plt>:
  403b20:	3c0f0041 	lui	t7,0x41
  403b24:	8df94018 	lw	t9,16408(t7)
  403b28:	03200008 	jr	t9
  403b2c:	25f84018 	addiu	t8,t7,16408

00403b30 <printf@plt>:
  403b30:	3c0f0041 	lui	t7,0x41
  403b34:	8df9401c 	lw	t9,16412(t7)
  403b38:	03200008 	jr	t9
  403b3c:	25f8401c 	addiu	t8,t7,16412

00403b40 <free@plt>:
  403b40:	3c0f0041 	lui	t7,0x41
  403b44:	8df94020 	lw	t9,16416(t7)
  403b48:	03200008 	jr	t9
  403b4c:	25f84020 	addiu	t8,t7,16416

00403b50 <IMP_FrameSource_DisableChn@plt>:
  403b50:	3c0f0041 	lui	t7,0x41
  403b54:	8df94024 	lw	t9,16420(t7)
  403b58:	03200008 	jr	t9
  403b5c:	25f84024 	addiu	t8,t7,16420

00403b60 <memcpy@plt>:
  403b60:	3c0f0041 	lui	t7,0x41
  403b64:	8df94028 	lw	t9,16424(t7)
  403b68:	03200008 	jr	t9
  403b6c:	25f84028 	addiu	t8,t7,16424

00403b70 <pthread_self@plt>:
  403b70:	3c0f0041 	lui	t7,0x41
  403b74:	8df9402c 	lw	t9,16428(t7)
  403b78:	03200008 	jr	t9
  403b7c:	25f8402c 	addiu	t8,t7,16428

00403b80 <time@plt>:
  403b80:	3c0f0041 	lui	t7,0x41
  403b84:	8df94030 	lw	t9,16432(t7)
  403b88:	03200008 	jr	t9
  403b8c:	25f84030 	addiu	t8,t7,16432

00403b90 <IMP_Encoder_CreateGroup@plt>:
  403b90:	3c0f0041 	lui	t7,0x41
  403b94:	8df94034 	lw	t9,16436(t7)
  403b98:	03200008 	jr	t9
  403b9c:	25f84034 	addiu	t8,t7,16436

00403ba0 <sleep@plt>:
  403ba0:	3c0f0041 	lui	t7,0x41
  403ba4:	8df94038 	lw	t9,16440(t7)
  403ba8:	03200008 	jr	t9
  403bac:	25f84038 	addiu	t8,t7,16440

00403bb0 <IMP_Encoder_DestroyGroup@plt>:
  403bb0:	3c0f0041 	lui	t7,0x41
  403bb4:	8df9403c 	lw	t9,16444(t7)
  403bb8:	03200008 	jr	t9
  403bbc:	25f8403c 	addiu	t8,t7,16444

00403bc0 <IMP_Encoder_Query@plt>:
  403bc0:	3c0f0041 	lui	t7,0x41
  403bc4:	8df94040 	lw	t9,16448(t7)
  403bc8:	03200008 	jr	t9
  403bcc:	25f84040 	addiu	t8,t7,16448

00403bd0 <IMP_System_Exit@plt>:
  403bd0:	3c0f0041 	lui	t7,0x41
  403bd4:	8df94044 	lw	t9,16452(t7)
  403bd8:	03200008 	jr	t9
  403bdc:	25f84044 	addiu	t8,t7,16452

00403be0 <IMP_ISP_DelSensor@plt>:
  403be0:	3c0f0041 	lui	t7,0x41
  403be4:	8df94048 	lw	t9,16456(t7)
  403be8:	03200008 	jr	t9
  403bec:	25f84048 	addiu	t8,t7,16456

00403bf0 <IMP_FrameSource_EnableChn@plt>:
  403bf0:	3c0f0041 	lui	t7,0x41
  403bf4:	8df9404c 	lw	t9,16460(t7)
  403bf8:	03200008 	jr	t9
  403bfc:	25f8404c 	addiu	t8,t7,16460

00403c00 <IMP_ISP_EnableTuning@plt>:
  403c00:	3c0f0041 	lui	t7,0x41
  403c04:	8df94050 	lw	t9,16464(t7)
  403c08:	03200008 	jr	t9
  403c0c:	25f84050 	addiu	t8,t7,16464

00403c10 <IMP_ISP_Close@plt>:
  403c10:	3c0f0041 	lui	t7,0x41
  403c14:	8df94054 	lw	t9,16468(t7)
  403c18:	03200008 	jr	t9
  403c1c:	25f84054 	addiu	t8,t7,16468

00403c20 <IMP_System_Init@plt>:
  403c20:	3c0f0041 	lui	t7,0x41
  403c24:	8df94058 	lw	t9,16472(t7)
  403c28:	03200008 	jr	t9
  403c2c:	25f84058 	addiu	t8,t7,16472

00403c30 <IMP_OSD_CreateGroup@plt>:
  403c30:	3c0f0041 	lui	t7,0x41
  403c34:	8df9405c 	lw	t9,16476(t7)
  403c38:	03200008 	jr	t9
  403c3c:	25f8405c 	addiu	t8,t7,16476

00403c40 <IMP_OSD_DestroyRgn@plt>:
  403c40:	3c0f0041 	lui	t7,0x41
  403c44:	8df94060 	lw	t9,16480(t7)
  403c48:	03200008 	jr	t9
  403c4c:	25f84060 	addiu	t8,t7,16480

00403c50 <pthread_create@plt>:
  403c50:	3c0f0041 	lui	t7,0x41
  403c54:	8df94064 	lw	t9,16484(t7)
  403c58:	03200008 	jr	t9
  403c5c:	25f84064 	addiu	t8,t7,16484

00403c60 <malloc@plt>:
  403c60:	3c0f0041 	lui	t7,0x41
  403c64:	8df94068 	lw	t9,16488(t7)
  403c68:	03200008 	jr	t9
  403c6c:	25f84068 	addiu	t8,t7,16488

00403c70 <puts@plt>:
  403c70:	3c0f0041 	lui	t7,0x41
  403c74:	8df9406c 	lw	t9,16492(t7)
  403c78:	03200008 	jr	t9
  403c7c:	25f8406c 	addiu	t8,t7,16492

00403c80 <strerror@plt>:
  403c80:	3c0f0041 	lui	t7,0x41
  403c84:	8df94070 	lw	t9,16496(t7)
  403c88:	03200008 	jr	t9
  403c8c:	25f84070 	addiu	t8,t7,16496

00403c90 <IMP_ISP_Tuning_SetSensorFPS@plt>:
  403c90:	3c0f0041 	lui	t7,0x41
  403c94:	8df94074 	lw	t9,16500(t7)
  403c98:	03200008 	jr	t9
  403c9c:	25f84074 	addiu	t8,t7,16500

00403ca0 <open@plt>:
  403ca0:	3c0f0041 	lui	t7,0x41
  403ca4:	8df94078 	lw	t9,16504(t7)
  403ca8:	03200008 	jr	t9
  403cac:	25f84078 	addiu	t8,t7,16504

00403cb0 <IMP_Encoder_RegisterChn@plt>:
  403cb0:	3c0f0041 	lui	t7,0x41
  403cb4:	8df9407c 	lw	t9,16508(t7)
  403cb8:	03200008 	jr	t9
  403cbc:	25f8407c 	addiu	t8,t7,16508

00403cc0 <strftime@plt>:
  403cc0:	3c0f0041 	lui	t7,0x41
  403cc4:	8df94080 	lw	t9,16512(t7)
  403cc8:	03200008 	jr	t9
  403ccc:	25f84080 	addiu	t8,t7,16512

00403cd0 <localtime@plt>:
  403cd0:	3c0f0041 	lui	t7,0x41
  403cd4:	8df94084 	lw	t9,16516(t7)
  403cd8:	03200008 	jr	t9
  403cdc:	25f84084 	addiu	t8,t7,16516

00403ce0 <IMP_OSD_CreateRgn@plt>:
  403ce0:	3c0f0041 	lui	t7,0x41
  403ce4:	8df94088 	lw	t9,16520(t7)
  403ce8:	03200008 	jr	t9
  403cec:	25f84088 	addiu	t8,t7,16520

00403cf0 <IMP_OSD_SetRgnAttr@plt>:
  403cf0:	3c0f0041 	lui	t7,0x41
  403cf4:	8df9408c 	lw	t9,16524(t7)
  403cf8:	03200008 	jr	t9
  403cfc:	25f8408c 	addiu	t8,t7,16524

00403d00 <IMP_ISP_Open@plt>:
  403d00:	3c0f0041 	lui	t7,0x41
  403d04:	8df94090 	lw	t9,16528(t7)
  403d08:	03200008 	jr	t9
  403d0c:	25f84090 	addiu	t8,t7,16528

00403d10 <write@plt>:
  403d10:	3c0f0041 	lui	t7,0x41
  403d14:	8df94094 	lw	t9,16532(t7)
  403d18:	03200008 	jr	t9
  403d1c:	25f84094 	addiu	t8,t7,16532

00403d20 <memset@plt>:
  403d20:	3c0f0041 	lui	t7,0x41
  403d24:	8df94098 	lw	t9,16536(t7)
  403d28:	03200008 	jr	t9
  403d2c:	25f84098 	addiu	t8,t7,16536

00403d30 <__errno_location@plt>:
  403d30:	3c0f0041 	lui	t7,0x41
  403d34:	8df9409c 	lw	t9,16540(t7)
  403d38:	03200008 	jr	t9
  403d3c:	25f8409c 	addiu	t8,t7,16540

00403d40 <IMP_Encoder_DestroyChn@plt>:
  403d40:	3c0f0041 	lui	t7,0x41
  403d44:	8df940a0 	lw	t9,16544(t7)
  403d48:	03200008 	jr	t9
  403d4c:	25f840a0 	addiu	t8,t7,16544

00403d50 <IMP_FrameSource_CreateChn@plt>:
  403d50:	3c0f0041 	lui	t7,0x41
  403d54:	8df940a4 	lw	t9,16548(t7)
  403d58:	03200008 	jr	t9
  403d5c:	25f840a4 	addiu	t8,t7,16548

00403d60 <IMP_ISP_DisableSensor@plt>:
  403d60:	3c0f0041 	lui	t7,0x41
  403d64:	8df940a8 	lw	t9,16552(t7)
  403d68:	03200008 	jr	t9
  403d6c:	25f840a8 	addiu	t8,t7,16552

00403d70 <pthread_exit@plt>:
  403d70:	3c0f0041 	lui	t7,0x41
  403d74:	8df940ac 	lw	t9,16556(t7)
  403d78:	03200008 	jr	t9
  403d7c:	25f840ac 	addiu	t8,t7,16556

00403d80 <IMP_Encoder_UnRegisterChn@plt>:
  403d80:	3c0f0041 	lui	t7,0x41
  403d84:	8df940b0 	lw	t9,16560(t7)
  403d88:	03200008 	jr	t9
  403d8c:	25f840b0 	addiu	t8,t7,16560

00403d90 <IMP_OSD_ShowRgn@plt>:
  403d90:	3c0f0041 	lui	t7,0x41
  403d94:	8df940b4 	lw	t9,16564(t7)
  403d98:	03200008 	jr	t9
  403d9c:	25f840b4 	addiu	t8,t7,16564

00403da0 <IMP_OSD_Start@plt>:
  403da0:	3c0f0041 	lui	t7,0x41
  403da4:	8df940b8 	lw	t9,16568(t7)
  403da8:	03200008 	jr	t9
  403dac:	25f840b8 	addiu	t8,t7,16568

00403db0 <IMP_OSD_DestroyGroup@plt>:
  403db0:	3c0f0041 	lui	t7,0x41
  403db4:	8df940bc 	lw	t9,16572(t7)
  403db8:	03200008 	jr	t9
  403dbc:	25f840bc 	addiu	t8,t7,16572

00403dc0 <IMP_FrameSource_DestroyChn@plt>:
  403dc0:	3c0f0041 	lui	t7,0x41
  403dc4:	8df940c0 	lw	t9,16576(t7)
  403dc8:	03200008 	jr	t9
  403dcc:	25f840c0 	addiu	t8,t7,16576

00403dd0 <IMP_FrameSource_SetChnAttr@plt>:
  403dd0:	3c0f0041 	lui	t7,0x41
  403dd4:	8df940c4 	lw	t9,16580(t7)
  403dd8:	03200008 	jr	t9
  403ddc:	25f840c4 	addiu	t8,t7,16580

00403de0 <IMP_ISP_AddSensor@plt>:
  403de0:	3c0f0041 	lui	t7,0x41
  403de4:	8df940c8 	lw	t9,16584(t7)
  403de8:	03200008 	jr	t9
  403dec:	25f840c8 	addiu	t8,t7,16584

00403df0 <sprintf@plt>:
  403df0:	3c0f0041 	lui	t7,0x41
  403df4:	8df940cc 	lw	t9,16588(t7)
  403df8:	03200008 	jr	t9
  403dfc:	25f840cc 	addiu	t8,t7,16588

00403e00 <IMP_Encoder_ReleaseStream@plt>:
  403e00:	3c0f0041 	lui	t7,0x41
  403e04:	8df940d0 	lw	t9,16592(t7)
  403e08:	03200008 	jr	t9
  403e0c:	25f840d0 	addiu	t8,t7,16592

00403e10 <IMP_System_UnBind@plt>:
  403e10:	3c0f0041 	lui	t7,0x41
  403e14:	8df940d4 	lw	t9,16596(t7)
  403e18:	03200008 	jr	t9
  403e1c:	25f840d4 	addiu	t8,t7,16596

00403e20 <IMP_Encoder_StartRecvPic@plt>:
  403e20:	3c0f0041 	lui	t7,0x41
  403e24:	8df940d8 	lw	t9,16600(t7)
  403e28:	03200008 	jr	t9
  403e2c:	25f840d8 	addiu	t8,t7,16600

00403e30 <IMP_OSD_RegisterRgn@plt>:
  403e30:	3c0f0041 	lui	t7,0x41
  403e34:	8df940dc 	lw	t9,16604(t7)
  403e38:	03200008 	jr	t9
  403e3c:	25f840dc 	addiu	t8,t7,16604

00403e40 <pthread_detach@plt>:
  403e40:	3c0f0041 	lui	t7,0x41
  403e44:	8df940e0 	lw	t9,16608(t7)
  403e48:	03200008 	jr	t9
  403e4c:	25f840e0 	addiu	t8,t7,16608

00403e50 <IMP_OSD_UpdateRgnAttrData@plt>:
  403e50:	3c0f0041 	lui	t7,0x41
  403e54:	8df940e4 	lw	t9,16612(t7)
  403e58:	03200008 	jr	t9
  403e5c:	25f840e4 	addiu	t8,t7,16612

00403e60 <IMP_Encoder_GetStream@plt>:
  403e60:	3c0f0041 	lui	t7,0x41
  403e64:	8df940e8 	lw	t9,16616(t7)
  403e68:	03200008 	jr	t9
  403e6c:	25f840e8 	addiu	t8,t7,16616

00403e70 <close@plt>:
  403e70:	3c0f0041 	lui	t7,0x41
  403e74:	8df940ec 	lw	t9,16620(t7)
  403e78:	03200008 	jr	t9
  403e7c:	25f840ec 	addiu	t8,t7,16620

00403e80 <IMP_Encoder_StopRecvPic@plt>:
  403e80:	3c0f0041 	lui	t7,0x41
  403e84:	8df940f0 	lw	t9,16624(t7)
  403e88:	03200008 	jr	t9
  403e8c:	25f840f0 	addiu	t8,t7,16624

00403e90 <IMP_Encoder_PollingStream@plt>:
  403e90:	3c0f0041 	lui	t7,0x41
  403e94:	8df940f4 	lw	t9,16628(t7)
  403e98:	03200008 	jr	t9
  403e9c:	25f840f4 	addiu	t8,t7,16628

#include "StdAfx.h"
#if OPENCV
#include "InspFunc.h"

#include "AlignModuleDlg.h"
#include "ModelData.h"


CInspFunc* pInsp;

CInspFunc::CInspFunc(void)
{
}


CInspFunc::~CInspFunc(void)
{
}

int CInspFunc::RegionSegmentation(unsigned char* pBuf_TargetImg, int BufSizeX, int BufSizeY, CRect* r, int minTh, int maxTh, int minlimitSizeX, int minlimitSizeY, int scanPixel, int drawFlag, COpenCV_LibDlg* pTmpDlg)
{
	//1Band 영상만 검사가능
	int x;
	int	i, count, lineStartPos = 0, yPos, L1;
	int	n_next, next[MAX_PNT_NO], merge;
	int	seed, seed_l;  //searching start seg. no and the line no of the seg
	int	n_seg, seg_n, s1;
	int	n_rgn, rgn_n, max_rgn = 0, n_candidate;
	unsigned long max_cnt;
	int min_cX;
	int min_cY;
	n_seg = 0;

	unsigned char* TargetImgBuf = new unsigned char[BufSizeX*BufSizeY];

	memset(l_snseg, -1, sizeof(l_snseg));
	memset(l_nseg, 0, sizeof(l_nseg));
	memset(&sp, 0, sizeof(sp));

	int posOffset = BufSizeX;

	int initX = r->left;
	int lastX = r->right;
	int initY = r->top;
	int lastY = r->bottom;

	int lngMinLimit = 2;
	int scanMergePixelX = scanPixel;
	int scanMergePixelY = scanPixel;

	if (initX < 5)                             initX = 5;
	if (lastX > (BufSizeX - 5)) lastX = (BufSizeX - 5);
	if (initY < 5)                             initY = 5;
	if (lastY > (BufSizeY - 5)) lastY = (BufSizeY - 5);

	if (drawFlag == 1)
	{
		scanMergePixelX = 10;
	}
	if (drawFlag == 2)
	{
		// 		lngMinLimit = minlimitSizeY/2; //최소 인식 Y사이즈
		scanMergePixelX = 300;
	}
	if (drawFlag == 3)
	{
		lngMinLimit = 100;
	}

	int pos;
	int dy;
	int offset_pixel = 0;

	int posEx = 0;

	CPoint point1[2];
	POINT p1, p2;

	for (x = initX; x < lastX; x += scanPixel)
	{
		dy = lastY - initY;
		l_nseg[x] = yPos = 0;
		l_snseg[x] = n_seg;
		count = 0;
		do
		{
			while (yPos < dy)
			{
				if (x<5 || x>(BufSizeX - 5) || (initY + yPos) < 5 || (initY + yPos) > (BufSizeY - 5)) break;
				pos = ((initY + yPos)*BufSizeX + x);

				// 				if (ImgBuff_Gray->imageData[pos] == 0)
				// 				{
				// 					yPos ++;
				// 					continue;;
				// 				}

				if (TargetImgBuf[pos] >= minTh && TargetImgBuf[pos] <= maxTh)
				{
					count = 1;
					lineStartPos = yPos;   //save start point of segment
					yPos++;
					break;
				}

				yPos += 2;
				// 				if (x<5 || x>(BufSizeX - 5) || (initY + yPos) < 5 || (initY + yPos) > (BufSizeY - 5)) break;
				// 				pos = ((initY + yPos)*BufSizeX + x);
			}
			while (yPos < dy)   //count length of segment	
			{
				if (x<5 || x>(BufSizeX - 5) || (initY + yPos) < 5 || (initY + yPos) > (BufSizeY - 5)) break;
				pos = ((initY + yPos)*BufSizeX + x);

				// 				if (ImgBuff_Gray->imageData[pos] == 0)
				// 				{
				// 					break;
				// // 					yPos += 2;
				// // 					if (x<5 || x>(BufSizeX - 5) || (initY + yPos) < 5 || (initY + yPos) > (BufSizeY - 5)) break;
				// // 					pos = ((initY + yPos)*BufSizeX + x);
				// // 
				// // 					if (Buf_TargetImg[pos]<minTh || Buf_TargetImg[pos]>maxTh)
				// // 						continue;
				// // 					else
				// // 						break;
				// 				}

				if (TargetImgBuf[pos] >= minTh && TargetImgBuf[pos] <= maxTh)
					count++;
				else
				{
					yPos += 2;
					if (x<5 || x>(BufSizeX - 5) || (initY + yPos) < 5 || (initY + yPos) > (BufSizeY - 5)) break;
					pos = ((initY + yPos)*BufSizeX + x);

					if (TargetImgBuf[pos]<minTh || TargetImgBuf[pos]>maxTh)
						continue;
					else
						break;
				}
				yPos++;
				// 				if (x<5 || x>(BufSizeX - 5) || (initY + yPos) < 5 || (initY + yPos) > (BufSizeY - 5)) break;
				// 				pos = ((initY + yPos)*BufSizeX + x);
			}
			if (lngMinLimit <= count)
			{
				seg_x[n_seg] = x;
				seg_y[n_seg] = initY + lineStartPos;
				seg_c[n_seg] = count;
				l_nseg[x]++;
				seg_f[n_seg] = ON;

				// 				if (m_bOverlay && !runningFlag)
				{
					p1.x = p2.x = x;
					p1.y = initY + lineStartPos;
					p2.y = initY + lineStartPos + count;

					CvPoint drawPt[2];
					drawPt[0].x = (int)p1.x;
					drawPt[0].y = (int)p1.y;
					drawPt[1].x = (int)p2.x;
					drawPt[1].y = (int)p2.y;
#ifdef _DEBUG
// 					if (!pView->m_bAutoRun)
					{
						if (drawFlag == 1)
							cvLib->DoDrawOverlay(pTmpDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_LIGHTRED), 3, 2);
						if (drawFlag == 2)
							cvLib->DoDrawOverlay(pTmpDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_LIGHTGREEN), 3, 2);
					}
#endif
				}

				count = 0;
				//increase segment buffer
				if (n_seg >= (MAX_PNT_NO - 1))
				{
					break;
				}
				n_seg++;
			}
		} while (yPos < dy);
	}

	double tmpCx, tmpCy;
	unsigned long tmpCnt;
	int minX = 10000;
	int maxX = 0;
	int minY = 10000;
	int maxY = 0;
	int maxSeed = 0;
	n_rgn = 0;
	while (1)
	{
		minX = 10000;
		maxX = 0;
		minY = 10000;
		maxY = 0;
		maxSeed = 0;

		tmpCnt = 0L;
		tmpCx = 0.0f;
		tmpCy = 0.0f;
		seed = -1;     //find seed segment 

		for (seg_n = 0; seg_n < n_seg; seg_n++)
		{// Find One Seed
			if (seg_f[seg_n] == ON)
			{
				seed = seg_n;
				break;
			}
		}
		if (seed == -1) break;   //if no seed is fount stop
		next[0] = seed;    // Seed Save
		n_next = 1;        //no. of segs that should be considered after current seed
		do   //continue to next seed
		{
			n_next--;
			if (n_next < 0) break;
			seed = next[n_next];
			seed_l = seg_x[seed];
			tmpCx += (double)(seg_x[seed] * seg_c[seed]);
			tmpCy += (double)((double)(seg_y[seed] + (double)((seg_c[seed] + 1) >> 1))*(double)seg_c[seed]);
			tmpCnt += (unsigned long)seg_c[seed];

			minX = minX > seg_x[seed] ? seg_x[seed] : minX;
			minY = minY > seg_y[seed] ? seg_y[seed] : minY;
			maxX = maxX < seg_x[seed] ? seg_x[seed] : maxX;
			maxY = maxY < (seg_y[seed] + seg_c[seed]) ? (seg_y[seed] + seg_c[seed]) : maxY;
			maxSeed = maxSeed < seed ? seed : maxSeed;


			seg_f[seed] = OFF;
			for (int tmpNo = 0; tmpNo < scanMergePixelX; tmpNo++)
			{
				if ((seed_l - (1 + tmpNo)) < 0) continue;
				for (L1 = 0; L1 < (int)l_nseg[seed_l - (1 + tmpNo)]; L1++)    //line above seed
				{
					s1 = (int)l_snseg[seed_l - (1 + tmpNo)] + L1;
					if (s1<0) continue;
					if (seg_f[s1] == OFF) continue;
					if (((int)seg_y[s1] + (int)seg_c[s1]) < 0 || ((int)seg_y[s1] + (int)seg_c[s1]) > BufSizeY ||
						((int)seg_y[seed] + (int)seg_c[seed]) < 0 || ((int)seg_y[seed] + (int)seg_c[seed]) > BufSizeY) continue;
					merge = CheckOverlap((int)seg_y[s1], (int)seg_c[s1], (int)seg_y[seed], (int)seg_c[seed]);
					if (merge == ON)
					{
						next[n_next++] = s1;
						if (n_next >= MAX_PNT_NO)
						{
							n_next = MAX_PNT_NO - 1;
							break;
						}
					}
				}// L2 for loop end
				if ((seed_l + (1 + tmpNo))>BufSizeX) continue;
				for (L1 = 0; L1 < (int)l_nseg[seed_l + (1 + tmpNo)]; L1++) //line below seed
				{
					s1 = (int)l_snseg[seed_l + (1 + tmpNo)] + L1;
					if (s1 < 0) continue;
					if (seg_f[s1] == OFF) continue;
					if (((int)seg_y[s1] + (int)seg_c[s1]) < 0 || ((int)seg_y[s1] + (int)seg_c[s1]) > BufSizeY ||
						((int)seg_y[seed] + (int)seg_c[seed]) < 0 || ((int)seg_y[seed] + (int)seg_c[seed]) > BufSizeY) continue;
					merge = CheckOverlap((int)seg_y[s1], (int)seg_c[s1], (int)seg_y[seed], (int)seg_c[seed]);
					if (merge == ON)
					{
						next[n_next++] = s1;
						if (n_next >= MAX_PNT_NO)
						{
							n_next = MAX_PNT_NO - 1;
							break; 
						}
					}
				} // L1 for loop end
			}
		} while (n_next>0);   //continue to next seed


		sp.min_area = 0;
		sp.max_area = 1000000;
		if ((tmpCnt > (unsigned long)sp.min_area) && (tmpCnt < (unsigned long)sp.max_area))
		{
			rgn_cx[n_rgn] = (int)((double)tmpCx / (double)tmpCnt);
			rgn_cy[n_rgn] = (int)((double)tmpCy / (double)tmpCnt);
			rgn_cnt[n_rgn] = tmpCnt;
			rgn_sx[n_rgn] = minX;
			rgn_sy[n_rgn] = minY;
			rgn_ex[n_rgn] = maxX;
			rgn_ey[n_rgn] = maxY;
			n_rgn++;

			if (n_rgn >= MAX_PNT_NO) break;
		}
	} //while(1) loop end
	sp.n_cand = n_rgn;
	//find max sp.n_cand of candidate
	n_candidate = 0;
	//crossList.cnt = 0;
	for (i = 0; i < sp.n_cand; i++)       //find three candidate
	{
		max_cnt = (unsigned long)0L;
		min_cX = 100000;
		min_cY = 100000;

		for (rgn_n = 0; rgn_n < n_rgn; rgn_n++)
		{
			if (rgn_cnt[rgn_n] == 0)
				continue;

			//if (rgn_cnt[rgn_n]>max_cnt)
			//{
			//	max_cnt = rgn_cnt[rgn_n];
			//	max_rgn = rgn_n;
			//}
			if (rgn_cy[rgn_n] < min_cY)
			{
				min_cY = rgn_cy[rgn_n];
				if (rgn_cx[rgn_n] < min_cX)
				{
					min_cX = rgn_cx[rgn_n];

					max_rgn = rgn_n;
				}
			}
		}

		if (rgn_cnt[max_rgn] == 0)
			continue;

		sp.center[n_candidate][0] = (int)rgn_cx[max_rgn]; // x_axis
		sp.center[n_candidate][1] = (int)rgn_cy[max_rgn]; // y_axis
		sp.center[n_candidate][2] = rgn_cnt[max_rgn];   //save the size of selected region
		sp.minX[n_candidate] = rgn_sx[max_rgn];
		sp.minY[n_candidate] = rgn_sy[max_rgn];
		sp.maxX[n_candidate] = rgn_ex[max_rgn];
		sp.maxY[n_candidate] = rgn_ey[max_rgn];
		sp.sizeX[n_candidate] = rgn_ex[max_rgn] - rgn_sx[max_rgn];
		sp.sizeY[n_candidate] = rgn_ey[max_rgn] - rgn_sy[max_rgn];

		rgn_cnt[max_rgn] = 0;

		if ((sp.sizeX[n_candidate] >= (int)(minlimitSizeX)) && (sp.sizeY[n_candidate] >= (int)(minlimitSizeY)))
		{
			n_candidate++;
		}

		if (n_candidate >= MAX_PNT_NO) break;
	}
	sp.n_region = n_candidate;

	if (n_candidate > 0) return n_candidate;
	else              return 0;
}

int CInspFunc::CheckOverlap(int s1, int n1, int s2, int n2)
{
	if (s1 >= s2)
	{
		if (s1 <= (s2 + n2)) return ON;
		else            return OFF;
	}
	else
	{
		if (s2 <= (s1 + n1)) return ON;
		else            return OFF;
	}
}

double CInspFunc::gradientAverage(int areaNo, POINT Point[MAX_PNT_NO], int cnt, int direction)
{
	// Least Square Linear Fitting ref) x=ay+b=>x_direction=0, y=ax+b=>y_direction=1 
	double xy_value = 0.0, x_value = 0.0, y_value = 0.0, a_value = 0.0, b_value = 0.0, yy_value = 0.0, xx_value = 0.0;
	CDPoint edge_pnt_sigma[MAX_PNT_NO];
	int i = 0, count = 0, count_sigma = 0;
	int maxX = -10000, minX = 10000, maxY = -10000, minY = 10000;
	double average = 0.0, average_x = 0.0, average_y = 0.0, average_sigma = 0.0, average_sigma_x = 0.0, divergence = 0.0, sigma = 0.0, edge_pnt_sigma_double[MAX_PNT_NO];

	for (i = 0; i < MAX_PNT_NO; i++)
	{
		edge_pnt_sigma[i].x = 0.0f;
		edge_pnt_sigma[i].y = 0.0f;
		edge_pnt_sigma_double[i] = 0.0;
	}

	// 		if (areaNo == 4)
	// 		{
	// 			result->leftEdgeCutCnt[board][camera][1] = 0;
	// 		}
	// 		if (areaNo == 5)
	// 		{
	// 			result->rightEdgeCutCnt[board][camera][1] = 0;
	// 		}

	for (i = 0; i < 8; i++)
	{
		result->lineValue[areaNo][i] = 0.0;
	}

	for (i = 0; i < cnt; i++)
	{
		if (Point[i].x<3.0f || Point[i].x>(float)(DEF_IMAGE_X - 3)) continue;
		if (Point[i].y<3.0f || Point[i].y>(float)(DEF_IMAGE_Y - 3)) continue;

		xy_value += (double)Point[i].x*(double)Point[i].y;
		x_value += (double)Point[i].x;
		y_value += (double)Point[i].y;
		if (!direction) yy_value += (double)Point[i].y*(double)Point[i].y;
		else           xx_value += (double)Point[i].x*(double)Point[i].x;
		maxX = maxX < Point[i].x ? Point[i].x : maxX;
		minX = minX > Point[i].x ? Point[i].x : minX;
		maxY = maxY < Point[i].y ? Point[i].y : maxY;
		minY = minY > Point[i].y ? Point[i].y : minY;
		count++;
		//		if(m_bOverlay) addPixelList(Point[i].x, Point[i].y, LIGHTCYAN);
	}

	if (count < 2 || cnt < 2)
	{
		return 0.0;
	}
	else if (((count*yy_value - y_value*y_value) == 0.0) || (count*xx_value - x_value*x_value) == 0.0)
	{
		return 0.0;
	}
	else
	{
		if (!direction)
		{
			a_value = ((count*xy_value) - (y_value*x_value)) / (count*yy_value - y_value*y_value);  // Gradient
			b_value = (-y_value*xy_value + yy_value*x_value) / (count*yy_value - y_value*y_value); // Y-Intercept    
		}
		else
		{
			a_value = ((count*xy_value) - (x_value*y_value)) / (count*xx_value - x_value*x_value);  // Gradient
			b_value = (-x_value*xy_value + xx_value*y_value) / (count*xx_value - x_value*x_value); // Y-Intercept    
		}
	}

	if (a_value == 0.0)	a_value = 0.00000001;
	if (a_value == 0.0)
	{
		if (direction)
		{
			POINT sl2, el2;
			sl2.x = minX;
			el2.x = maxX;
			sl2.y = (int)(a_value*sl2.x + b_value + 0.5);
			el2.y = (int)(a_value*el2.x + b_value + 0.5);

			CvPoint drawPt[2];
			drawPt[0].x = (int)sl2.x;
			drawPt[0].y = (int)sl2.y;
			drawPt[1].x = (int)el2.x;
			drawPt[1].y = (int)el2.y;
			cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_RED), 3, 2);
			// 				if (m_bOverlay && (areaNo == 2 || areaNo == 3))
			// 					addLineList(sl2, el2, YELLOW);
			//    		if(m_bOverlay&&(areaNo==2||areaNo==3||areaNo==6||areaNo==7||areaNo==10||areaNo==11)) addLineList(sl2, el2, LIGHTRED);
		}
		else if (!direction)
		{
			POINT sl4, el4;
			sl4.y = minY;
			sl4.x = (int)(a_value*sl4.y + b_value);
			el4.y = maxY;
			el4.x = (int)(a_value*el4.y + b_value);

			CvPoint drawPt[2];
			drawPt[0].x = (int)sl4.x;
			drawPt[0].y = (int)sl4.y;
			drawPt[1].x = (int)el4.x;
			drawPt[1].y = (int)el4.y;
			cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_RED), 3, 2);
			//			if(m_bOverlay&&(areaNo==4||areaNo==9||areaNo==0||areaNo==1)) addLineList(sl4, el4, LIGHTRED);
		}

		if (!direction)
		{
			result->lineValue[areaNo][0] = a_value;
			result->lineValue[areaNo][1] = b_value;
			result->lineValue[areaNo][2] = count_sigma;
			result->lineValue[areaNo][3] = b_value;
			result->lineValue[areaNo][4] = b_value;
			result->lineValue[areaNo][5] = minY;
			result->lineValue[areaNo][6] = maxY;
			result->lineValueBodyGap[areaNo][5] = minY;
			result->lineValueBodyGap[areaNo][6] = maxY;
			return b_value;
		}
		else
		{
			result->lineValue[areaNo][0] = a_value;
			result->lineValue[areaNo][1] = b_value;
			result->lineValue[areaNo][2] = count_sigma;
			result->lineValue[areaNo][3] = minX;
			result->lineValue[areaNo][4] = maxX;
			result->lineValue[areaNo][5] = b_value;
			result->lineValue[areaNo][6] = b_value;
			result->lineValueBodyGap[areaNo][5] = b_value;
			result->lineValueBodyGap[areaNo][6] = b_value;
			return b_value;
		}
	}
	else
	{
		for (i = 0; i < cnt; i++)
		{
			if (!direction) edge_pnt_sigma_double[i] = fabs((double)Point[i].x - ((double)(a_value*Point[i].y) + b_value));
			else           edge_pnt_sigma_double[i] = fabs((double)Point[i].y - ((double)(a_value*Point[i].x) + b_value));
			edge_pnt_sigma[i].x = (float)Point[i].x;
			edge_pnt_sigma[i].y = (float)Point[i].y;
			average_sigma += edge_pnt_sigma_double[i];
		}
		average_sigma = (average_sigma / (double)cnt);

		for (i = 0; i < cnt; i++)
		{
			divergence += (edge_pnt_sigma_double[i] - average_sigma)*(edge_pnt_sigma_double[i] - average_sigma);
		}
		if (cnt >= 20) divergence = (divergence / (double)cnt);
		else       divergence = (divergence / (double)(cnt - 1));

		sigma = sqrt(divergence);
		result->sigmaValue = sigma;
		count_sigma = 0;
		for (i = 0; i < cnt; i++)
		{
			if (edge_pnt_sigma_double[i] <= (average_sigma + 0.5*sigma))
			{
				edge_pnt_sigma_double[count_sigma] = edge_pnt_sigma_double[i];
				edge_pnt_sigma[count_sigma].x = edge_pnt_sigma[i].x;
				edge_pnt_sigma[count_sigma].y = edge_pnt_sigma[i].y;

				// 					if (areaNo == 4)
				// 					{
				// 						result->leftEdgeCut[board][count_sigma][1].x = edge_pnt_sigma[i].x;
				// 						result->leftEdgeCut[board][count_sigma][1].y = edge_pnt_sigma[i].y;
				// 					}
				// 					if (areaNo == 5)
				// 					{
				// 						result->rightEdgeCut[board][count_sigma][1].x = edge_pnt_sigma[i].x;
				// 						result->rightEdgeCut[board][count_sigma][1].y = edge_pnt_sigma[i].y;
				// 					}
				count_sigma++;
			}
		}

		// 			if (areaNo == 4)
		// 			{
		// 				result->leftEdgeCutCnt[board][1] = count_sigma;
		// 			}
		// 			if (areaNo == 5)
		// 			{
		// 				result->rightEdgeCutCnt[board][1] = count_sigma;
		// 			}


		for (i = 0; i < count_sigma; i++)
		{
			average_sigma_x += edge_pnt_sigma[i].x;
		}
		average_sigma_x = (average_sigma_x / (double)count_sigma); // Line to Point Sigma Average

		for (i = 0; i < cnt; i++)
		{
			if (!direction) average += ((double)(Point[i].y*a_value) + b_value);
			else           average += ((double)(Point[i].x*a_value) + b_value);
		}
		if (!direction) average_x = (average / (float)cnt);  // Line Fitting Average
		else           average_y = (average / (float)cnt);
	}

	xy_value = x_value = y_value = a_value = b_value = xx_value = yy_value = average = average_x = average_y = 0.0;
	count = 0, maxX = -10000, minX = 10000, maxY = -10000, minY = 10000;
	for (i = 0; i < count_sigma; i++)
	{
		if (edge_pnt_sigma[i].x<3.0f || edge_pnt_sigma[i].x>(float)(DEF_IMAGE_X - 3)) continue;
		if (edge_pnt_sigma[i].y<3.0f || edge_pnt_sigma[i].y>(float)(DEF_IMAGE_Y - 3)) continue;

		xy_value += (double)edge_pnt_sigma[i].x*(double)edge_pnt_sigma[i].y;
		x_value += (double)edge_pnt_sigma[i].x;
		y_value += (double)edge_pnt_sigma[i].y;
		if (!direction) yy_value += (double)edge_pnt_sigma[i].y*(double)edge_pnt_sigma[i].y;
		else           xx_value += (double)edge_pnt_sigma[i].x*(double)edge_pnt_sigma[i].x;

		maxX = maxX < (int)edge_pnt_sigma[i].x ? (int)edge_pnt_sigma[i].x : maxX;
		minX = minX > (int)edge_pnt_sigma[i].x ? (int)edge_pnt_sigma[i].x : minX;
		maxY = maxY < (int)edge_pnt_sigma[i].y ? (int)edge_pnt_sigma[i].y : maxY;
		minY = minY > (int)edge_pnt_sigma[i].y ? (int)edge_pnt_sigma[i].y : minY;
		count++;
		//		if(m_bOverlay) addPixelList(edge_pnt_sigma[i].x, edge_pnt_sigma[i].y, LIGHTCYAN);
	}

	if (count == 0 || count_sigma == 0) return 0.0;
	else if (((count*yy_value - y_value*y_value) == 0.0) || (count*xx_value - x_value*x_value) == 0.0)
	{
		return 0.0;
	}
	else
	{
		if (!direction)
		{
			a_value = ((count*xy_value) - (y_value*x_value)) / (count*yy_value - y_value*y_value);  // Gradient
			b_value = (-y_value*xy_value + yy_value*x_value) / (count*yy_value - y_value*y_value); // Y-Intercept    
		}
		else
		{
			a_value = ((count*xy_value) - (x_value*y_value)) / (count*xx_value - x_value*x_value);  // Gradient
			b_value = (-x_value*xy_value + xx_value*y_value) / (count*xx_value - x_value*x_value); // Y-Intercept    
		}
	}


	if (a_value == 0.0)	a_value = 0.00000001;
	if (a_value == 0.0)
	{
		if (direction)
		{
			POINT sl1, el1;
			sl1.x = minX;
			el1.x = maxX;
			sl1.y = (int)(a_value*sl1.x + b_value + 0.5);
			el1.y = (int)(a_value*el1.x + b_value + 0.5);

			CvPoint drawPt[2];
			drawPt[0].x = (int)sl1.x;
			drawPt[0].y = (int)sl1.y;
			drawPt[1].x = (int)el1.x;
			drawPt[1].y = (int)el1.y;
			cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_GREEN), 3, 2);
			// 				if (m_bOverlay && (areaNo == 2 || areaNo == 3))
			// 					addLineList(sl1, el1, YELLOW);
			//       		if(m_bOverlay&&(areaNo==2||areaNo==3||areaNo==6||areaNo==7||areaNo==10||areaNo==11)) addLineList(sl1, el1, LIGHTRED);
		}
		else if (!direction)
		{
			POINT sl3, el3;
			sl3.y = minY;
			sl3.x = (int)(a_value*sl3.y + b_value);
			el3.y = maxY;
			el3.x = (int)(a_value*el3.y + b_value);

			CvPoint drawPt[2];
			drawPt[0].x = (int)sl3.x;
			drawPt[0].y = (int)sl3.y;
			drawPt[1].x = (int)el3.x;
			drawPt[1].y = (int)el3.y;
			cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_GREEN), 3, 2);
			//			if(m_bOverlay&&(areaNo==4||areaNo==9||areaNo==0||areaNo==1)) addLineList(sl3, el3, LIGHTRED);
		}

		if (!direction)
		{
			result->lineValue[areaNo][0] = a_value;
			result->lineValue[areaNo][1] = b_value;
			result->lineValue[areaNo][2] = count_sigma;
			result->lineValue[areaNo][3] = b_value;
			result->lineValue[areaNo][4] = b_value;
			result->lineValue[areaNo][5] = minY;
			result->lineValue[areaNo][6] = maxY;
			result->lineValueBodyGap[areaNo][5] = minY;
			result->lineValueBodyGap[areaNo][6] = maxY;
			return b_value;
		}
		else
		{
			result->lineValue[areaNo][0] = a_value;
			result->lineValue[areaNo][1] = b_value;
			result->lineValue[areaNo][2] = count_sigma;
			result->lineValue[areaNo][3] = minX;
			result->lineValue[areaNo][4] = maxX;
			result->lineValue[areaNo][5] = b_value;
			result->lineValue[areaNo][6] = b_value;
			result->lineValueBodyGap[areaNo][5] = b_value;
			result->lineValueBodyGap[areaNo][6] = b_value;
			return b_value;
		}
	}
	else
	{
		for (i = 0; i < count_sigma; i++)
		{
			if (!direction) average += ((double)(edge_pnt_sigma[i].y*a_value) + b_value);
			else           average += ((double)(edge_pnt_sigma[i].x*a_value) + b_value);
		}
		if (!direction) average_x = (average / (float)count_sigma); // Line Fitting+Sigma Average
		else           average_y = (average / (float)count_sigma);
	}

	result->lineValue[areaNo][0] = a_value;
	result->lineValue[areaNo][1] = b_value;
	result->lineValue[areaNo][2] = count_sigma;
	if (!direction)
	{
		if (a_value > 0.0)
		{
			result->lineValue[areaNo][3] = (a_value*minY + b_value);//minX;
			result->lineValue[areaNo][4] = (a_value*maxY + b_value);//maxX;
			result->lineValue[areaNo][5] = minY;//((minX-b_value)/a_value);
			result->lineValue[areaNo][6] = maxY;//((maxX-b_value)/a_value);
			result->lineValueBodyGap[areaNo][5] = minY;//((minX-b_value)/a_value);
			result->lineValueBodyGap[areaNo][6] = maxY;//((maxX-b_value)/a_value);
		}
		else
		{
			result->lineValue[areaNo][3] = (a_value*maxY + b_value);//minX;
			result->lineValue[areaNo][4] = (a_value*minY + b_value);//maxX;
			result->lineValue[areaNo][5] = maxY;//((maxX-b_value)/a_value); 
			result->lineValue[areaNo][6] = minY;//((minX-b_value)/a_value);
			result->lineValueBodyGap[areaNo][5] = maxY;//((maxX-b_value)/a_value); 
			result->lineValueBodyGap[areaNo][6] = minY;//((minX-b_value)/a_value);
		}
	}
	else if (direction)
	{
		if (a_value > 0.0)
		{
			result->lineValue[areaNo][3] = minX;//((minY-b_value)/a_value);
			result->lineValue[areaNo][4] = maxX;//((maxY-b_value)/a_value);
			result->lineValue[areaNo][5] = (a_value*minX + b_value);//minY;
			result->lineValue[areaNo][6] = (a_value*maxX + b_value);//maxY;
			result->lineValueBodyGap[areaNo][5] = (a_value*minX + b_value);//minY;
			result->lineValueBodyGap[areaNo][6] = (a_value*maxX + b_value);//maxY;
		}
		else
		{
			result->lineValue[areaNo][3] = maxX;//((maxY-b_value)/a_value);
			result->lineValue[areaNo][4] = minX;//((minY-b_value)/a_value);
			result->lineValue[areaNo][5] = (a_value*maxX + b_value);//minY;
			result->lineValue[areaNo][6] = (a_value*minX + b_value);//maxY;
			result->lineValueBodyGap[areaNo][5] = (a_value*minX + b_value);//minY;
			result->lineValueBodyGap[areaNo][6] = (a_value*maxX + b_value);//maxY;
		}
	}

	if (!direction)
	{
		POINT sl, el;
		sl.y = minY;
		sl.x = (int)(a_value*sl.y + b_value);
		el.y = maxY;
		el.x = (int)(a_value*el.y + b_value);

		CvPoint drawPt[2];
		drawPt[0].x = (int)sl.x;
		drawPt[0].y = (int)sl.y;
		drawPt[1].x = (int)el.x;
		drawPt[1].y = (int)el.y;
		cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_LIGHTCYAN), 3, 2);
		// 			if (m_bOverlay && (areaNo == 4 || areaNo == 5 || areaNo == 9 || areaNo == 0 || areaNo == 1))
		// 				addLineList(sl, el, LIGHTRED);
	}
	else
	{
		POINT sl, el;
		sl.x = minX;
		el.x = maxX;
		sl.y = (int)(a_value*sl.x + b_value + 0.5);
		el.y = (int)(a_value*el.x + b_value + 0.5);

		CvPoint drawPt[2];
		drawPt[0].x = (int)sl.x;
		drawPt[0].y = (int)sl.y;
		drawPt[1].x = (int)el.x;
		drawPt[1].y = (int)el.y;
		cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_LIGHTCYAN), 3, 2);
		//		if(m_bOverlay&&(areaNo==2||areaNo==3||areaNo==6||areaNo==7||areaNo==10||areaNo==11)) addLineList(sl, el, LIGHTRED);
		// 			if (m_bOverlay && (areaNo == 2 || areaNo == 3))
		// 				addLineList(sl, el, YELLOW);
	}

	if (!direction && (average_x != 0.0))
	{
		// 			if (modelData->chipInfo[0] == 1)
		// 			{
		// 				if (areaNo == 1 || areaNo == 4 || areaNo == 5)      return result->lineValue[board][areaNo][3];
		// 				else if (areaNo == 0 || areaNo == 9 || areaNo == 8) return result->lineValue[board][areaNo][4];
		// 			}
		// 			else
		{
			return 0.5*(result->lineValue[areaNo][3] + result->lineValue[areaNo][4]);
		}
	}
	else if (direction && (average_y != 0.0))
	{
		//		if(areaNo==2)      return result->lineValue[board][areaNo][5];
		//		else if(areaNo==3) return result->lineValue[board][areaNo][6];
		// 			if (modelData->chipInfo[0] == 1)
		// 			{
		// 				return average_y;
		// 			}
		// 			else
		{
			return 0.5*(result->lineValue[areaNo][5] + result->lineValue[areaNo][6]);
		}
	}

	return 0.0;
}

int CInspFunc::SearchEdge(int imgeBand, int imgSizeX, int imgSizeY, int colorIndex, unsigned char *ImageBuffer, CRect r, int area, int diffTh, int diffPixel, int val, int valDir, int scandir, int overlayColor, float sigmaSTD)
{
	if (imgeBand == 1)
		colorIndex = 2;

	register int x, y;
	int sx, sy, ex, ey;
	int pos, pos1, pos2, pos3, pos4;
	int pos5, pos6, pos7, pos8, pos9;
	int pos_1, pos1_1, pos2_1, pos3_1, pos4_1;
	int pos_2, pos1_2, pos2_2, pos3_2, pos4_2;
	int value0, value1, value2, value3;
	int value0_1, value0_3, value1_1, value1_3;
	int i;
	CDPoint fPoint[DEF_IMAGE_X];
	int direction = 0;
	double gradient, yValue;
	int ScanPixel = 1;

	BOOL bFindEdge;
	CvPoint drawPt[2];

	int edgeColor = CYAN;
	if (overlayColor > 0)
		edgeColor = overlayColor;

	CvScalar cvOverlayColor[19] = {
		 cvScalar(CV_BLACK)
		,cvScalar(CV_BLUE)
		,cvScalar(CV_GREEN)
		,cvScalar(CV_CYAN)
		,cvScalar(CV_RED)
		,cvScalar(CV_MAGENTA)
		,cvScalar(CV_BROWN)
		,cvScalar(CV_DARKGRAY)
		,cvScalar(CV_LIGHTGRAY)
		,cvScalar(CV_LIGHTBLUE)
		,cvScalar(CV_LIGHTGREEN)
		,cvScalar(CV_LIGHTCYAN)
		,cvScalar(CV_LIGHTRED)
		,cvScalar(CV_LIGHTMAGENTA)
		,cvScalar(CV_YELLOW)
		,cvScalar(CV_WHITE)
		,cvScalar(CV_DLGCOLOR)
		,cvScalar(CV_ORANGE)
		,cvScalar(CV_LIGHTORANGE)
	};

	r.left = r.left < 0 ? 0 : r.left;
	r.right = r.right > imgSizeX ? imgSizeX : r.right;
	r.top = r.top < 0 ? 0 : r.top;
	r.bottom = r.bottom > imgSizeY ? imgSizeY : r.bottom;

	sx = r.left;
	ex = r.right;
	sy = r.top;
	ey = r.bottom < (int)(imgSizeY*0.85) ? r.bottom : (int)(imgSizeY*0.85);//너무 아래로 쳐지면 검색 못하게 막음
	if (sx < imgeBand*diffPixel)	sx = imgeBand*(diffPixel + 5);
	if (sy < imgeBand*diffPixel)	sy = imgeBand*(diffPixel + 5);
	if (ex > imgSizeX - imgeBand*diffPixel)	ex = imgSizeX - imgeBand*(diffPixel + 5);
	if (ey > imgSizeY - imgeBand*diffPixel)	ey = imgSizeY - imgeBand*(diffPixel + 5);

	scandir = 0;//고정값
	switch (scandir)
	{
	case 0:
		{	// x:+  y:+ ↓
			direction = 1;
			memset(resEdgePoint[area], 0x00, sizeof(CDPoint) * DEF_IMAGE_X);
			resEdgeCnt[area] = 0;
			for (x = sx; x < ex; x += ScanPixel)
			{
				for (y = sy; y < ey; y += ScanPixel)
				{
					if((x - diffPixel)<0 || (x + diffPixel)>imgSizeX )
						continue;
					if ((y - diffPixel)<0 || (y + diffPixel)>imgSizeY)
						continue;
					//////////////////////////////////////////////////////////////////////////
					//pos2_1	pos2	pos2_2
					//Pos4_1	pos4	pos4_2
					//pos_1		pos		pos_2
					//Pos3_1	pos3	pos3_2
					//pos1_1	pos1	pos1_2
					//////////////////////////////////////////////////////////////////////////
					pos = imgeBand*(y*imgSizeX + x) + (2 - ((colorIndex + 0) % 3));
					pos1 = imgeBand*((y + diffPixel)*imgSizeX + (x)) + (2 - ((colorIndex + 0) % 3));
					pos2 = imgeBand*((y - diffPixel)*imgSizeX + (x)) + (2 - ((colorIndex + 0) % 3));
					pos3 = imgeBand*((y + 1)*imgSizeX + (x)) + (2 - ((colorIndex + 0) % 3));
					pos4 = imgeBand*((y - 1)*imgSizeX + (x)) + (2 - ((colorIndex + 0) % 3));

					//2-(colorIndex+0)%3 : R/G/B
					//2-(colorIndex+1)%3 : G/B/R
					//2-(colorIndex+2)%3 : B/R/G
					if (imgeBand == 3)
					{
						pos5 = imgeBand*(y*imgSizeX + x) + (2 - ((colorIndex + 2) % 3));
						pos6 = imgeBand*((y + diffPixel)*imgSizeX + (x)) + (2 - ((colorIndex + 2) % 3));
						pos7 = imgeBand*((y - diffPixel)*imgSizeX + (x)) + (2 - ((colorIndex + 2) % 3));
						pos8 = imgeBand*((y + 1)*imgSizeX + (x)) + (2 - ((colorIndex + 2) % 3));
						pos9 = imgeBand*((y - 1)*imgSizeX + (x)) + (2 - ((colorIndex + 2) % 3));
					}
					else
					{
						pos5 = 0;
						pos6 = 0;
						pos7 = 0;
						pos8 = 0;
						pos9 = 0;
					}

					pos_1 = pos - imgeBand*diffPixel;
					pos1_1 = pos1 - imgeBand*diffPixel;
					pos2_1 = pos2 - imgeBand*diffPixel;
					pos3_1 = pos3 - imgeBand*diffPixel;
					pos4_1 = pos4 - imgeBand*diffPixel;

					pos_2 = pos + imgeBand*diffPixel;
					pos1_2 = pos1 + imgeBand*diffPixel;
					pos2_2 = pos2 + imgeBand*diffPixel;
					pos3_2 = pos3 + imgeBand*diffPixel;
					pos4_2 = pos4 + imgeBand*diffPixel;


					if (imgeBand == 3)
					{
						value0 = (ImageBuffer[pos] - ImageBuffer[pos5]) < 0 ? 0 : (ImageBuffer[pos] - ImageBuffer[pos5]);
						value1 = abs((ImageBuffer[pos] - ImageBuffer[pos5]) - (ImageBuffer[pos2] - ImageBuffer[pos7]));
					}
					else
					{
						value0 = (ImageBuffer[pos]) < 0 ? 0 : (ImageBuffer[pos]);
						value1 = abs((ImageBuffer[pos]) - (ImageBuffer[pos2]));
					}
					value2 = abs(ImageBuffer[pos1_1] - ImageBuffer[pos2_1]);
					value3 = abs(ImageBuffer[pos1_2] - ImageBuffer[pos2_2]);
					value0_1 = ImageBuffer[pos1];	//+
					value0_3 = ImageBuffer[pos3];	//+
					value1_1 = ImageBuffer[pos2];	//-
					value1_3 = ImageBuffer[pos4];	//-

					bFindEdge = FALSE;
					switch (valDir)
					{
					case 0:
						{	//↓
							if (value0 < val && (value1 > diffTh))//|| value2>diffTh || value3>diffTh))
							{
								bFindEdge = TRUE;
							}
						}
						break;
					default:
						{
							if (value0 > val && (value1 > diffTh))//|| value2 > diffTh || value3 > diffTh))
							{
								bFindEdge = TRUE;
							}
						}
						break;
					}

					if(bFindEdge)
					{
						sy = y - 50;//성능향상***

						resEdgePoint[area][resEdgeCnt[area]].x = (double)x;
						resEdgePoint[area][resEdgeCnt[area]].y = (double)y;
						resEdgeCnt[area]++;
						// #ifdef _DEBUG
						if (overlayColor >= 0)
						{
							drawPt[0].x = x;
							drawPt[0].y = y;
							cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_CROSS, drawPt, cvOverlayColor[overlayColor%19], 1, 1, 2);
						}
						// #endif
						break;
					}
				}
			}

			if ((ex - sx)*0.1f > resEdgeCnt[area] * ScanPixel)
			{
				if (area == 1)		// 2 Edge
				{
					resEdgeCnt[area] = 0;

					resEdgePoint[area][resEdgeCnt[area]].x = (double)resPartRect.left;
					resEdgePoint[area][resEdgeCnt[area]].y = (double)resPartRect.bottom;
					resEdgeCnt[area]++;
					resEdgePoint[area][resEdgeCnt[area]].x = (double)resPartRect.right;
					resEdgePoint[area][resEdgeCnt[area]].y = (double)resPartRect.bottom;
					resEdgeCnt[area]++;
				}
				else
				{
					// 				swprintf_s(szMessage, sizeof(wchar_t)*MESSAGE_SIZE, L"[%s:%d]%s%s[%02d Edge]",lang[DISPLAY_PRINT]->TextData[57],CamNo+1,lang[DISPLAY_PRINT]->TextData[12],lang[DISPLAY_PRINT]->TextData[71],area+1);
					// 			swprintf_s(szMessage, sizeof(wchar_t)*MESSAGE_SIZE, L"[Camera:%d]엣지 검출 미달[%02d 엣지]", CamNo+1,area+1);
					return (1000 + (area + 1) * 10);

				}
			}

// 			for (i = 0; i < resEdgeCnt[area]; i++)
// 			{
// 				fPoint[i].x = (float)resEdgePoint[area][i].x;
// 				fPoint[i].y = (float)resEdgePoint[area][i].y;
// 			}
			
		}
		break;
	case 1:
		{	// x:+  y:- ↑
			direction = 1;
			memset(resEdgePoint[area], 0x00, sizeof(CDPoint) * DEF_IMAGE_X);
			resEdgeCnt[area] = 0;
			for (x = sx; x < ex; x += ScanPixel)
			{
				for (y = ey; y > sy; y -= ScanPixel)
				{
					pos = imgeBand*(y*imgSizeX + x) + (2 - colorIndex);
					pos1 = imgeBand*((y + diffPixel)*imgSizeX + (x)) + (2 - colorIndex);
					pos2 = imgeBand*((y - diffPixel)*imgSizeX + (x)) + (2 - colorIndex);
					pos3 = imgeBand*((y + 1)*imgSizeX + (x)) + (2 - colorIndex);
					pos4 = imgeBand*((y - 1)*imgSizeX + (x)) + (2 - colorIndex);

					value0 = ImageBuffer[pos];
					value1 = abs(ImageBuffer[pos2] - ImageBuffer[pos1]);
					value2 = abs(ImageBuffer[pos4] - ImageBuffer[pos3]);
					value0_1 = ImageBuffer[pos1];	//+
					value0_3 = ImageBuffer[pos3];	//+
					value1_1 = ImageBuffer[pos2];	//-
					value1_3 = ImageBuffer[pos4];	//-

					if (valDir == 0)
					{	//↓
						if (value0 < val /*&& value1_1 < val*/ && value1>diffTh && (value0_1 - value0) > diffTh)
						{
							resEdgePoint[area][resEdgeCnt[area]].x = x;
							resEdgePoint[area][resEdgeCnt[area]].y = y;
							resEdgeCnt[area]++;
							// #ifdef _DEBUG
// 							if (overlayColor >= 0)
// 								pDisplay[DEF_DISP_SIDE_SET]->addPixelList(x, y/* - 40*/, edgeColor, TRUE);
							// #endif
							break;
						}
					}
					else
					{
						if (value0 > val /*&& value1_1 > val*/ && value1 > diffTh && (value0_1 - value0) > diffTh)
						{
							resEdgePoint[area][resEdgeCnt[area]].x = x;
							resEdgePoint[area][resEdgeCnt[area]].y = y;
							resEdgeCnt[area]++;
							// #ifdef _DEBUG
// 							if (overlayColor >= 0)
// 								pDisplay[DEF_DISP_SIDE_SET]->addPixelList(x, y/* - 40*/, edgeColor, TRUE);
							// #endif
							break;
						}
					}
				}
			}
			if ((ex - sx)*0.1f > resEdgeCnt[area] * ScanPixel)
			{
				if (area == 0)		// 1 Edge
				{
					resEdgeCnt[area] = 0;

					resEdgePoint[area][resEdgeCnt[area]].x = resPartRect.left;
					resEdgePoint[area][resEdgeCnt[area]].y = resPartRect.top;
					resEdgeCnt[area]++;
					resEdgePoint[area][resEdgeCnt[area]].x = resPartRect.right;
					resEdgePoint[area][resEdgeCnt[area]].y = resPartRect.top;
					resEdgeCnt[area]++;

				}
				else
				{
					// 				swprintf_s(szMessage, sizeof(wchar_t)*MESSAGE_SIZE, L"[%s:%d]%s%s[%02d Edge]",lang[DISPLAY_PRINT]->TextData[57],CamNo+1,lang[DISPLAY_PRINT]->TextData[12],lang[DISPLAY_PRINT]->TextData[71],area+1);
					// 			swprintf_s(szMessage, sizeof(wchar_t)*MESSAGE_SIZE, L"[Camera:%d]엣지 검출 미달[%02d 엣지]", CamNo+1,area+1);
					return (1000 + (area + 1) * 10);

				}
			}
			for (i = 0; i < resEdgeCnt[area]; i++)
			{
				fPoint[i].x = (float)resEdgePoint[area][i].x;
				fPoint[i].y = (float)resEdgePoint[area][i].y;
			}
			LinegradientAverage_Double(imgSizeX, imgSizeY, fPoint, resEdgeCnt[area], direction, area, sigmaSTD, &gradient, &yValue);

		}
		break;
	case 2:
		{	// x:+  y:+ →
			direction = 0;
			memset(resEdgePoint[area], 0x00, sizeof(CDPoint) * DEF_IMAGE_X);
			resEdgeCnt[area] = 0;
			for (y = sy; y < ey; y += ScanPixel)
			{
				for (x = sx; x < ex; x += ScanPixel)
				{
					pos = imgeBand*(y*imgSizeX + x) + (2 - colorIndex);
					pos1 = imgeBand*((y)*imgSizeX + (x + diffPixel)) + (2 - colorIndex);
					pos2 = imgeBand*((y)*imgSizeX + (x - diffPixel)) + (2 - colorIndex);
					pos3 = imgeBand*((y)*imgSizeX + (x + 1)) + (2 - colorIndex);
					pos4 = imgeBand*((y)*imgSizeX + (x - 1)) + (2 - colorIndex);

					value0 = ImageBuffer[pos];
					value1 = abs(ImageBuffer[pos1] - ImageBuffer[pos2]);
					value2 = abs(ImageBuffer[pos3] - ImageBuffer[pos4]);
					value0_1 = ImageBuffer[pos1];	//+
					value0_3 = ImageBuffer[pos3];	//+
					value1_1 = ImageBuffer[pos2];	//-
					value1_3 = ImageBuffer[pos4];	//-

					if (valDir == 0)
					{	//↓
						if (value0 < val && value0_1<val && value1>diffTh /*&& value2>modelData[bdNo]->edgeTh*/)
						{
							resEdgePoint[area][resEdgeCnt[area]].x = x;
							resEdgePoint[area][resEdgeCnt[area]].y = y;
							resEdgeCnt[area]++;
							// 						if(!g_autoFlag && m_bOverlay)	milclassTmp->addCrossList_DC(x,y,2, edgeColor);
							break;
						}
					}
					else
					{
						if (value0 > val && value0_1 > val && value1 > diffTh /*&& value2>modelData[bdNo]->edgeTh*/)
						{
							resEdgePoint[area][resEdgeCnt[area]].x = x;
							resEdgePoint[area][resEdgeCnt[area]].y = y;
							resEdgeCnt[area]++;
							// 						if(!g_autoFlag && m_bOverlay)	milclassTmp->addCrossList_DC(x,y,2, edgeColor);
							break;
						}
					}
				}
			}
			if ((ey - sy)*0.1f > resEdgeCnt[area] * ScanPixel)
			{
				if (area == 3)		// 4 Edge
				{
					resEdgeCnt[area] = 0;

					resEdgePoint[area][resEdgeCnt[area]].x = resPartRect.right;
					resEdgePoint[area][resEdgeCnt[area]].y = resPartRect.top;
					resEdgeCnt[area]++;
					resEdgePoint[area][resEdgeCnt[area]].x = resPartRect.right;
					resEdgePoint[area][resEdgeCnt[area]].y = resPartRect.bottom;
					resEdgeCnt[area]++;
				}
				else
				{
					// 				swprintf_s(szMessage, sizeof(wchar_t)*MESSAGE_SIZE, L"[%s:%d]%s%s[%02d Edge]",lang[DISPLAY_PRINT]->TextData[57],CamNo+1,lang[DISPLAY_PRINT]->TextData[12],lang[DISPLAY_PRINT]->TextData[71],area+1);
					// 			swprintf_s(szMessage, sizeof(wchar_t)*MESSAGE_SIZE, L"[Camera:%d]엣지 검출 미달[%02d 엣지]", CamNo+1,area+1);
					return (1000 + (area + 1) * 10);

				}
			}
			for (i = 0; i < resEdgeCnt[area]; i++)
			{
				fPoint[i].x = (float)resEdgePoint[area][i].x;
				fPoint[i].y = (float)resEdgePoint[area][i].y;
			}
			LinegradientAverage_Double(imgSizeX, imgSizeY, fPoint, resEdgeCnt[area], direction, area, sigmaSTD, &gradient, &yValue);
		}
		break;
	case 3:
		{	// x:+  y:+ ←
			direction = 0;
			memset(resEdgePoint[area], 0x00, sizeof(CDPoint) * DEF_IMAGE_X);
			resEdgeCnt[area] = 0;
			for (y = sy; y < ey; y += ScanPixel)
			{
				for (x = ex; x > sx; x -= ScanPixel)
				{
					pos = imgeBand*(y*imgSizeX + x) + (2 - colorIndex);
					pos1 = imgeBand*((y)*imgSizeX + (x + diffPixel)) + (2 - colorIndex);
					pos2 = imgeBand*((y)*imgSizeX + (x - diffPixel)) + (2 - colorIndex);
					pos3 = imgeBand*((y)*imgSizeX + (x + 1)) + (2 - colorIndex);
					pos4 = imgeBand*((y)*imgSizeX + (x - 1)) + (2 - colorIndex);

					value0 = ImageBuffer[pos];
					value1 = abs(ImageBuffer[pos2] - ImageBuffer[pos1]);
					value2 = abs(ImageBuffer[pos4] - ImageBuffer[pos3]);
					value0_1 = ImageBuffer[pos1];	//+
					value0_3 = ImageBuffer[pos3];	//+
					value1_1 = ImageBuffer[pos2];	//-
					value1_3 = ImageBuffer[pos4];	//-

					if (valDir == 0)
					{	//↓
						if (value0 < val && value1_1<val && value1>diffTh /*&& value2>modelData[bdNo]->edgeTh*/)
						{
							resEdgePoint[area][resEdgeCnt[area]].x = x;
							resEdgePoint[area][resEdgeCnt[area]].y = y;
							resEdgeCnt[area]++;
							// 						if(!g_autoFlag && m_bOverlay)	milclassTmp->addCrossList_DC(x,y,2, edgeColor);
							break;
						}
					}
					else
					{
						if (value0 > val && value1_1 > val && value1 > diffTh /*&& value2>modelData[bdNo]->edgeTh*/)
						{
							resEdgePoint[area][resEdgeCnt[area]].x = x;
							resEdgePoint[area][resEdgeCnt[area]].y = y;
							resEdgeCnt[area]++;
							// 						if(!g_autoFlag && m_bOverlay)	milclassTmp->addCrossList_DC(x,y,2, edgeColor);
							break;
						}
					}
				}
			}
			if ((ey - sy)*0.1f > resEdgeCnt[area] * ScanPixel)
			{
				if (area == 2)		// 3 Edge
				{
					resEdgeCnt[area] = 0;

					resEdgePoint[area][resEdgeCnt[area]].x = resPartRect.left;
					resEdgePoint[area][resEdgeCnt[area]].y = resPartRect.top;
					resEdgeCnt[area]++;
					resEdgePoint[area][resEdgeCnt[area]].x = resPartRect.left;
					resEdgePoint[area][resEdgeCnt[area]].y = resPartRect.bottom;
					resEdgeCnt[area]++;
				}
				else
				{
					// 				swprintf_s(szMessage, sizeof(wchar_t)*MESSAGE_SIZE, L"[%s:%d]%s%s[%02d Edge]",lang[DISPLAY_PRINT]->TextData[57], CamNo+1,lang[DISPLAY_PRINT]->TextData[12],lang[DISPLAY_PRINT]->TextData[71],area+1);
					// 			swprintf_s(szMessage, sizeof(wchar_t)*MESSAGE_SIZE, L"[Camera:%d]엣지 검출 미달[%02d 엣지]", CamNo+1,area+1);
					return (1000 + (area + 1) * 10);

				}
			}
			for (i = 0; i < resEdgeCnt[area]; i++)
			{
				fPoint[i].x = (float)resEdgePoint[area][i].x;
				fPoint[i].y = (float)resEdgePoint[area][i].y;
			}

			LinegradientAverage_Double(imgSizeX, imgSizeY, fPoint, resEdgeCnt[area], direction, area, sigmaSTD, &gradient, &yValue);

		}
		break;
	default:
		break;
	}


	return 9900;
}

double CInspFunc::LinegradientAverage_Double(int imgSizeX, int imgSizeY, CDPoint Point[], int cnt, int direction, int areaNo, float sigmaSTD, double* gradient, double* yValue, BOOL nOverlay/*=FALSE*/)
{
	// Least Square Linear Fitting ref) x=ay+b=>x_direction=0, y=ax+b=>y_direction=1 
	double xy_value = 0.0, x_value = 0.0, y_value = 0.0, a_value = 0.0, b_value = 0.0, yy_value = 0.0, xx_value = 0.0;
	CDPoint edge_pnt_sigma[MAX_PNT_NO];
	CDPoint pPoint[MAX_PNT_NO];
	register int i = 0, count = 0, count_sigma = 0;
	// 	int maxX=-1000, minX=1000, maxY=-1000, minY=1000; 
	double maxX = -10000, minX = 10000, maxY = -10000, minY = 10000;
	double average = 0.0, average_x = 0.0, average_y = 0.0, average_sigma = 0.0, average_sigma_x = 0.0, divergence = 0.0, sigma = 0.0, edge_pnt_sigma_double[MAX_PNT_NO];
	int buffCnt = 0;

	for (i = 0; i < cnt; i++)
	{
		pPoint[buffCnt++] = Point[i];
	}
	cnt = buffCnt;

	for (i = 0; i < MAX_PNT_NO; i++)
	{
		edge_pnt_sigma[i].x = 0.0f;
		edge_pnt_sigma[i].y = 0.0f;
		edge_pnt_sigma_double[i] = 0.0;
	}

	for (i = 0; i < cnt; i++)
	{
		if (pPoint[i].x<0.0f || pPoint[i].x>(float)(imgSizeX)) continue;
		if (pPoint[i].y<0.0f || pPoint[i].y>(float)(imgSizeY)) continue;

		xy_value += (double)pPoint[i].x*(double)pPoint[i].y;
		x_value += (double)pPoint[i].x;
		y_value += (double)pPoint[i].y;
		if (!direction) yy_value += (double)pPoint[i].y*(double)pPoint[i].y;
		else           xx_value += (double)pPoint[i].x*(double)pPoint[i].x;
		maxX = maxX < pPoint[i].x ? pPoint[i].x : (double)maxX;
		minX = minX > pPoint[i].x ? pPoint[i].x : (double)minX;
		maxY = maxY < pPoint[i].y ? pPoint[i].y : (double)maxY;
		minY = minY > pPoint[i].y ? pPoint[i].y : (double)minY;
		count++;
	}
	if (count < 2 || cnt < 2)
	{
		return 0.0;
	}
	else
	{
		if (!direction)
		{
			a_value = ((count*xy_value) - (y_value*x_value)) / (count*yy_value - y_value*y_value);  // Gradient
			b_value = (-y_value*xy_value + yy_value*x_value) / (count*yy_value - y_value*y_value); // Y-Intercept    
		}
		else
		{
			a_value = ((count*xy_value) - (x_value*y_value)) / (count*xx_value - x_value*x_value);  // Gradient
			b_value = (-x_value*xy_value + xx_value*y_value) / (count*xx_value - x_value*x_value); // Y-Intercept    
		}
	}

	if (a_value == 0.0)	a_value = 0.00000001;
	if (a_value == 0.0)
	{
		if (direction)
		{
			POINT sl2, el2;
			sl2.x = (int)minX;
			el2.x = (int)maxX;
			sl2.y = (int)(a_value*sl2.x + b_value + 0.5);
			el2.y = (int)(a_value*el2.x + b_value + 0.5);
			// 			if(!m_bOverlay)	milclassTmp->addLineList_DC(sl2, el2, MIL_LIGHTGREEN);

			CvPoint drawPt[2];
			drawPt[0].x = (int)sl2.x;
			drawPt[0].y = (int)sl2.y;
			drawPt[1].x = (int)el2.x;
			drawPt[1].y = (int)el2.y;
			cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_RED), 3, 2);
		}
		else if (!direction)
		{
			POINT sl4, el4;
			sl4.y = (int)minY;
			sl4.x = (int)(a_value*sl4.y + b_value);
			el4.y = (int)maxY;
			el4.x = (int)(a_value*el4.y + b_value);
			// 			if(!m_bOverlay)	milclassTmp->addLineList_DC(sl4, el4, MIL_LIGHTGREEN);

			CvPoint drawPt[2];
			drawPt[0].x = (int)sl4.x;
			drawPt[0].y = (int)sl4.y;
			drawPt[1].x = (int)el4.x;
			drawPt[1].y = (int)el4.y;
			cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_RED), 3, 2);
		}

		if (!direction)
		{
			lineValue[areaNo][0] = a_value;
			lineValue[areaNo][1] = b_value;
			lineValue[areaNo][2] = count_sigma;
			lineValue[areaNo][3] = b_value;
			lineValue[areaNo][4] = b_value;
			lineValue[areaNo][5] = minY;
			lineValue[areaNo][6] = maxY;
			return b_value;
		}
		else
		{
			lineValue[areaNo][0] = a_value;
			lineValue[areaNo][1] = b_value;
			lineValue[areaNo][2] = count_sigma;
			lineValue[areaNo][3] = minX;
			lineValue[areaNo][4] = maxX;
			lineValue[areaNo][5] = b_value;
			lineValue[areaNo][6] = b_value;
			return b_value;
		}

		if (!direction)
		{
			*yValue = b_value;
			*gradient = 0.0;
			return b_value;
		}
		else
		{
			*yValue = b_value;
			*gradient = 0.0;
			return b_value;
		}
	}
	else
	{
		for (i = 0; i < cnt; i++)
		{
			if (!direction) edge_pnt_sigma_double[i] = fabs((double)pPoint[i].x - ((double)(a_value*pPoint[i].y) + b_value));
			else           edge_pnt_sigma_double[i] = fabs((double)pPoint[i].y - ((double)(a_value*pPoint[i].x) + b_value));
			edge_pnt_sigma[i].x = (float)pPoint[i].x;
			edge_pnt_sigma[i].y = (float)pPoint[i].y;
			average_sigma += edge_pnt_sigma_double[i];
		}
		average_sigma = (average_sigma / (double)cnt);

		for (i = 0; i < cnt; i++)
		{
			divergence = divergence + (edge_pnt_sigma_double[i] - average_sigma)*(edge_pnt_sigma_double[i] - average_sigma);
		}
		if (cnt >= 20)	divergence = (divergence / (double)(cnt));
		else		divergence = (divergence / (double)(cnt - 1));

		sigma = sqrt(divergence);
		count_sigma = 0;
		for (i = 0; i < cnt; i++)
		{
			if (!direction) edge_pnt_sigma_double[i] = fabs(((double)Point[i].x - ((double)(a_value*pPoint[i].y) + b_value)));
			else           edge_pnt_sigma_double[i] = fabs(((double)Point[i].y - ((double)(a_value*pPoint[i].x) + b_value)));

			if ( (edge_pnt_sigma_double[i] >= average_sigma - sigmaSTD*sigma && edge_pnt_sigma_double[i] <= average_sigma + sigmaSTD*sigma)
				|| sigma < sigmaSTD)
			{
				edge_pnt_sigma_double[count_sigma] = edge_pnt_sigma_double[i];
				edge_pnt_sigma[count_sigma].x = edge_pnt_sigma[i].x;
				edge_pnt_sigma[count_sigma].y = edge_pnt_sigma[i].y;

				Sigma_Point[areaNo][count_sigma].x = edge_pnt_sigma[i].x;
				Sigma_Point[areaNo][count_sigma].y = edge_pnt_sigma[i].y;

				count_sigma++;
				Sigma_PointCnt[areaNo] = count_sigma;
			}

			// 			if(edge_pnt_sigma_double[i]<=(average_sigma+1.5*sigma))
			// 			{
			// 				edge_pnt_sigma_double[count_sigma] = edge_pnt_sigma_double[i];
			// 				edge_pnt_sigma[count_sigma].x = edge_pnt_sigma[i].x;
			// 				edge_pnt_sigma[count_sigma].y = edge_pnt_sigma[i].y;
			// 				count_sigma++;
			// 			}
		}

		for (i = 0; i < count_sigma; i++)
		{
			average_sigma_x += edge_pnt_sigma[i].x;
		}
		average_sigma_x = (average_sigma_x / (double)count_sigma); // Line to Point Sigma Average

		for (i = 0; i < cnt; i++)
		{
			// 			if(!direction) average +=((double)(Point[i].y*a_value)+b_value);
			// 			else           average +=((double)(Point[i].x*a_value)+b_value);
			if (!direction) average += ((double)(edge_pnt_sigma[i].y*a_value) + b_value);
			else           average += ((double)(edge_pnt_sigma[i].x*a_value) + b_value);
		}
		if (!direction) average_x = (average / (float)cnt);  // Line Fitting Average
		else           average_y = (average / (float)cnt);
	}

	xy_value = x_value = y_value = a_value = b_value = xx_value = yy_value = average = average_x = average_y = 0.0;
	count = 0, maxX = -10000, minX = 10000, maxY = -10000, minY = 10000;
	for (i = 0; i < count_sigma; i++)
	{
		if (edge_pnt_sigma[i].x<0.0f || edge_pnt_sigma[i].x>(float)(imgSizeX)) continue;
		if (edge_pnt_sigma[i].y<0.0f || edge_pnt_sigma[i].y>(float)(imgSizeY)) continue;

		xy_value += (double)edge_pnt_sigma[i].x*(double)edge_pnt_sigma[i].y;
		x_value += (double)edge_pnt_sigma[i].x;
		y_value += (double)edge_pnt_sigma[i].y;
		if (!direction) yy_value += (double)edge_pnt_sigma[i].y*(double)edge_pnt_sigma[i].y;
		else           xx_value += (double)edge_pnt_sigma[i].x*(double)edge_pnt_sigma[i].x;

		maxX = maxX < (int)edge_pnt_sigma[i].x ? (int)edge_pnt_sigma[i].x : maxX;
		minX = minX > (int)edge_pnt_sigma[i].x ? (int)edge_pnt_sigma[i].x : minX;
		maxY = maxY < (int)edge_pnt_sigma[i].y ? (int)edge_pnt_sigma[i].y : maxY;
		minY = minY > (int)edge_pnt_sigma[i].y ? (int)edge_pnt_sigma[i].y : minY;
		count++;
	}

	if (count == 0 || count_sigma == 0) return 0.0;
	else
	{
		if (!direction)
		{
			a_value = ((count*xy_value) - (y_value*x_value)) / (count*yy_value - y_value*y_value);  // Gradient
			b_value = (-y_value*xy_value + yy_value*x_value) / (count*yy_value - y_value*y_value); // Y-Intercept    
		}
		else
		{
			a_value = ((count*xy_value) - (x_value*y_value)) / (count*xx_value - x_value*x_value);  // Gradient
			b_value = (-x_value*xy_value + xx_value*y_value) / (count*xx_value - x_value*x_value); // Y-Intercept    
		}
	}

	if (a_value == 0.0)	a_value = 0.00000001;
	if (a_value == 0.0)
	{
		if (direction)
		{
			POINT sl1, el1;
			sl1.x = (int)minX;
			el1.x = (int)maxX;
			sl1.y = (int)(a_value*sl1.x + b_value + 0.5);
			el1.y = (int)(a_value*el1.x + b_value + 0.5);
			// 			if(!m_bOverlay)	milclassTmp->addLineList_DC(sl1, el1, MIL_YELLOW);
			CvPoint drawPt[2];
			drawPt[0].x = (int)sl1.x;
			drawPt[0].y = (int)sl1.y;
			drawPt[1].x = (int)el1.x;
			drawPt[1].y = (int)el1.y;
			cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_GREEN), 3, 2);
		}
		else if (!direction)
		{
			POINT sl3, el3;
			sl3.y = (int)minY;
			sl3.x = (int)(a_value*sl3.y + b_value);
			el3.y = (int)maxY;
			el3.x = (int)(a_value*el3.y + b_value);
			// 			if(!m_bOverlay)	milclassTmp->addLineList_DC(sl3, el3, MIL_YELLOW);
			CvPoint drawPt[2];
			drawPt[0].x = (int)sl3.x;
			drawPt[0].y = (int)sl3.y;
			drawPt[1].x = (int)el3.x;
			drawPt[1].y = (int)el3.y;
			cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_GREEN), 3, 2);
		}
		if (!direction)
		{
			lineValue[areaNo][0] = a_value;
			lineValue[areaNo][1] = b_value;
			lineValue[areaNo][2] = count_sigma;
			lineValue[areaNo][3] = b_value;
			lineValue[areaNo][4] = b_value;
			lineValue[areaNo][5] = minY;
			lineValue[areaNo][6] = maxY;
			return b_value;
		}
		else
		{
			lineValue[areaNo][0] = a_value;
			lineValue[areaNo][1] = b_value;
			lineValue[areaNo][2] = count_sigma;
			lineValue[areaNo][3] = minX;
			lineValue[areaNo][4] = maxX;
			lineValue[areaNo][5] = b_value;
			lineValue[areaNo][6] = b_value;
			return b_value;
		}

		if (!direction)
		{
			*yValue = b_value;
			*gradient = 0.0;
			return b_value;
		}
		else
		{
			*yValue = b_value;
			*gradient = 0.0;
			return b_value;
		}
	}
	else
	{
		for (i = 0; i < count_sigma; i++)
		{
			if (!direction) average += ((double)(edge_pnt_sigma[i].y*a_value) + b_value);
			else           average += ((double)(edge_pnt_sigma[i].x*a_value) + b_value);
		}
		if (!direction) average_x = (average / (float)count_sigma); // Line Fitting+Sigma Average
		else           average_y = (average / (float)count_sigma);
	}
	lineValue[areaNo][0] = a_value;
	lineValue[areaNo][1] = b_value;
	lineValue[areaNo][2] = count_sigma;
	if (!direction)
	{
		if (a_value > 0.0)
		{
			lineValue[areaNo][3] = (a_value*minY + b_value);//minX;
			lineValue[areaNo][4] = (a_value*maxY + b_value);//maxX;
			lineValue[areaNo][5] = minY;//((minX-b_value)/a_value);
			lineValue[areaNo][6] = maxY;//((maxX-b_value)/a_value);
		}
		else
		{
			lineValue[areaNo][3] = (a_value*maxY + b_value);//minX;
			lineValue[areaNo][4] = (a_value*minY + b_value);//maxX;
			lineValue[areaNo][5] = maxY;//((maxX-b_value)/a_value); 
			lineValue[areaNo][6] = minY;//((minX-b_value)/a_value);
		}
	}
	else if (direction)
	{
		if (a_value > 0.0)
		{
			lineValue[areaNo][3] = minX;//((minY-b_value)/a_value);
			lineValue[areaNo][4] = maxX;//((maxY-b_value)/a_value);
			lineValue[areaNo][5] = (a_value*minX + b_value);//minY;
			lineValue[areaNo][6] = (a_value*maxX + b_value);//maxY;
		}
		else
		{
			lineValue[areaNo][3] = maxX;//((maxY-b_value)/a_value);
			lineValue[areaNo][4] = minX;//((minY-b_value)/a_value);
			lineValue[areaNo][5] = (a_value*maxX + b_value);//minY;
			lineValue[areaNo][6] = (a_value*minX + b_value);//maxY;
		}
	}

	*yValue = b_value;
	*gradient = a_value;


	if (!direction)
	{
		POINT sl, el;
		sl.y = (int)minY + 100;
		sl.x = (int)(a_value*sl.y + b_value);
		el.y = (int)maxY - 100;
		el.x = (int)(a_value*el.y + b_value);
// 		if (nOverlay)	pDisplay[DEF_DISP_SIDE_SET]->addLineList(CPoint(sl.x, sl.y), CPoint(el.x, el.y), YELLOW, TRUE);
		CvPoint drawPt[2];
		drawPt[0].x = (int)sl.x;
		drawPt[0].y = (int)sl.y;
		drawPt[1].x = (int)el.x;
		drawPt[1].y = (int)el.y;
		cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_LIGHTCYAN), 3, 2);
	}
	else
	{
		POINT sl, el;
		sl.x = (int)maxX - 100;
		el.x = (int)minX + 100;
		sl.y = (int)(a_value*sl.x + b_value + 0.5);
		el.y = (int)(a_value*el.x + b_value + 0.5);
// 		if(nOverlay)	pDisplay[DEF_DISP_SIDE_SET]->addLineList(CPoint(sl.x,sl.y), CPoint(el.x,el.y), LIGHTMAGENTA, TRUE);
		CvPoint drawPt[2];
		drawPt[0].x = (int)sl.x;
		drawPt[0].y = (int)sl.y;
		drawPt[1].x = (int)el.x;
		drawPt[1].y = (int)el.y;
		cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_LINE, drawPt, cvScalar(CV_LIGHTCYAN), 3, 2);
	}

	if (!direction && (average_x != 0.0))
	{
		// 		if(areaNo==1 || areaNo==4 || areaNo==5)      return lineValue[areaNo][areaNo][3];
		// 		else if(areaNo==0 || areaNo==9 || areaNo==8) return lineValue[areaNo][areaNo][4];
		return average_x;
	}
	else if (direction && (average_y != 0.0))
	{
		//		if(areaNo==2)      return lineValue[areaNo][areaNo][5];
		//		else if(areaNo==3) return lineValue[areaNo][areaNo][6];
		return average_y;
	}

	return 0.0;
}

double CInspFunc::Parabola_gradientAverage_Double(int imgSizeX, int imgSizeY, CDPoint Point[], int cnt, int direction, int areaNo, float sigmaSTD, bool m_bOverlay /*= FALSE*/)
{
	double XXXXi = 0.0, XXXi = 0.0, XXi = 0.0, Xi = 0.0, XXYi = 0.0, XYi = 0.0, Yi = 0.0, YYYYi = 0.0, YYYi = 0.0, YYi = 0.0, YYXi = 0.0,
		A, B, C, D, xApex, yApex = 0.0;
	int i = 0, count = 0, count_sigma = 0, maxX = -10000, minX = 10000, maxY = -10000, minY = 10000;
	double average_sigma = 0.0, divergence = 0.0, sigma = 0.0, re_average_sigma = 0.0, re_divergence = 0.0, re_sigma = 0.0;
	double maxXApex = -10000.0, minXApex = 10000.0;
	CDPoint edge_pnt_sigma[MAX_PNT_NO];
	CDPoint Point1[MAX_PNT_NO];
	CvPoint drawPt[2];

	for (i = 0; i < MAX_PNT_NO; i++)
	{
		edge_pnt_sigma[i].x = 0.0f;
		edge_pnt_sigma[i].y = 0.0f;
		Point1[i].x = 0.0f;
		Point1[i].y = 0.0f;
	}

	if (cnt < 2)
	{
		return 0.0;
	}
	else //if(areaNo==10 || areaNo==11 || areaNo==12 || areaNo==13 || areaNo==14 || areaNo==15)
	{
		for (i = 0; i < cnt; i++)
		{
			average_sigma += Point[i].y;
		}
		average_sigma = (average_sigma / (double)cnt);

		for (i = 0; i < cnt; i++)
		{
			divergence = divergence + (Point[i].y - average_sigma)*(Point[i].y - average_sigma);
		}

		if ((cnt - 1) != 0) divergence = (divergence / (double)(cnt - 1));
		else           return 0.0;

		sigma = sqrt(divergence);
	}

	for (i = 0; i < cnt; i++)
	{
		if ((Point[i].y >= (float)(average_sigma - sigmaSTD*sigma)) && (Point[i].y <= (float)(average_sigma + sigmaSTD*sigma)))
		{
			edge_pnt_sigma[count_sigma].x = (float)Point[i].x;
			edge_pnt_sigma[count_sigma].y = (float)Point[i].y;
			re_average_sigma += (float)Point[i].y;

// 			Sigma_Point[areaNo][count_sigma].x = edge_pnt_sigma[count_sigma].x;
// 			Sigma_Point[areaNo][count_sigma].y = edge_pnt_sigma[count_sigma].y;

			count_sigma++;
// 			Sigma_PointCnt[areaNo] = count_sigma;
		}
		else
		{
// 			drawPt[0].x = (int)Point[i].x;
// 			drawPt[0].y = (int)Point[i].y;
// 			cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_CROSS, drawPt, CV_YELLOW, 1, 1, 2);
// 
// 			Sigma_Point[areaNo][count_sigma].x = (float)Point[i].x;
// 			Sigma_Point[areaNo][count_sigma].y = (float)Point[i].y;
// 
// 			count_sigma++;
// 			Sigma_PointCnt[areaNo] = count_sigma;
		}
	}
	if (count_sigma != 0)
	{
		re_average_sigma = (re_average_sigma / (double)count_sigma);

		cnt = count_sigma;
	}
	else  return 0.0;

	if (count_sigma < 2) return 0.0;
	else
	{
		for (i = 0; i < cnt; i++)
		{
			Point1[i].x = edge_pnt_sigma[i].x;
			Point1[i].y = edge_pnt_sigma[i].y;
			re_divergence = re_divergence + (Point1[i].y - re_average_sigma)*(Point1[i].y - re_average_sigma);
		}

		if ((cnt - 1) != 0) re_divergence = (re_divergence / (double)(cnt - 1));
		else           return 0.0;

		re_sigma = sqrt(re_divergence);
// 		pResult[m_DispNum]->dParabol_sigma = re_sigma;
	}

	for (i = 0; i < cnt; i++)
	{
		if (Point1[i].x<3.0f || Point1[i].x>(float)(imgSizeX - 3.0f)) continue;
		if (Point1[i].y<3.0f || Point1[i].y>(float)(imgSizeY - 3.0f)) continue;

		if (!direction)
		{// X=AY^2+BY+C
			YYYYi += (double)Point1[i].y*(double)Point1[i].y*(double)Point1[i].y*(double)Point1[i].y;
			YYYi += (double)Point1[i].y*(double)Point1[i].y*(double)Point1[i].y;
			YYi += (double)Point1[i].y*(double)Point1[i].y;
			YYXi += (double)Point1[i].y*(double)Point1[i].y*(double)Point1[i].x;
		}
		else
		{// Y=AX^2+BX+C
			XXXXi += (double)Point1[i].x*(double)Point1[i].x*(double)Point1[i].x*(double)Point1[i].x;
			XXXi += (double)Point1[i].x*(double)Point1[i].x*(double)Point1[i].x;
			XXi += (double)Point1[i].x*(double)Point1[i].x;
			XXYi += (double)Point1[i].x*(double)Point1[i].x*(double)Point1[i].y;
		}
		Xi += (double)Point1[i].x;
		Yi += (double)Point1[i].y;
		XYi += (double)Point1[i].x*(double)Point1[i].y;

		maxX = maxX < (int)Point1[i].x ? (int)Point1[i].x : maxX;
		minX = minX > (int)Point1[i].x ? (int)Point1[i].x : minX;
		maxY = maxY < (int)Point1[i].y ? (int)Point1[i].y : maxY;
		minY = minY > (int)Point1[i].y ? (int)Point1[i].y : minY;
		count++;
	}

	if (cnt == 0 || count == 0) return 0.0;
	else
	{
		if (!direction)
		{
			D = (YYYYi*YYi*count) + (YYYi*Yi*YYi) + (YYi*YYYi*Yi) - ((YYi*YYi*YYi) + (YYYYi*Yi*Yi) + (YYYi*YYYi*count)); //Determinant 
			if (D == 0.0) return 0.0;
			A = (((count*YYi - Yi*Yi)*YYXi) - ((count*YYYi - Yi*YYi)*XYi) + ((YYYi*Yi - YYi*YYi)*Xi)) / D;                 // Y^2
			B = (-((count*YYYi - Yi*YYi)*YYXi) + ((count*YYYYi - YYi*YYi)*XYi) - ((YYYYi*Yi - YYYi*YYi)*Xi)) / D;          // Y
			C = (((YYYi*Yi - YYi*YYi)*YYXi) - ((YYYYi*Yi - YYi*YYYi)*XYi) + ((YYYYi*YYi - YYYi*YYYi)*Xi)) / D;             // Constant
		}
		else
		{
			D = (XXXXi*XXi*count) + (XXXi*Xi*XXi) + (XXi*XXXi*Xi) - ((XXi*XXi*XXi) + (XXXXi*Xi*Xi) + (XXXi*XXXi*count)); //Determinant 
			if (D == 0.0) return 0.0;
			A = (((count*XXi - Xi*Xi)*XXYi) - ((count*XXXi - Xi*XXi)*XYi) + ((XXXi*Xi - XXi*XXi)*Yi)) / D;                 // X^2
			B = (-((count*XXXi - Xi*XXi)*XXYi) + ((count*XXXXi - XXi*XXi)*XYi) - ((XXXXi*Xi - XXXi*XXi)*Yi)) / D;          // X
			C = (((XXXi*Xi - XXi*XXi)*XXYi) - ((XXXXi*Xi - XXi*XXXi)*XYi) + ((XXXXi*XXi - XXXi*XXXi)*Yi)) / D;             // Constant
		}

		if (1)//areaNo==10 || areaNo==11 || areaNo==12 || areaNo==13 || areaNo==14 || areaNo==15)
		{
			if (!direction)
			{
				for (i = minY; i < maxY; i++)
				{
					// Y = Ax2+Bx+C
					if (m_bOverlay)
					{
// 						pDisplay[DEF_DISP_SIDE_SET]->addCrossList((int)(A*i*i + B*i + C), i - 100, 1, LIGHTGREEN, TRUE);
						drawPt[0].x = (int)(A*i*i + B*i + C);
						drawPt[0].y = i;
						cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_CROSS, drawPt, cvScalar(CV_LIGHTMAGENTA), 1, 1, 2);
					}
				}
			}
			else
			{
				// #ifdef _DEBUG
				minX = 10;
				maxX = imgSizeX - 10;
				// #endif
				for (i = minX; i < maxX; i++)
				{
					if (m_bOverlay)
					{
// 						pDisplay[DEF_DISP_SIDE_SET]->addPixelList(i, (int)(A*i*i + B*i + C) -100, LIGHTMAGENTA, TRUE);
						drawPt[0].x = i;
						drawPt[0].y = (int)(A*i*i + B*i + C);
						cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_CROSS, drawPt, cvScalar(CV_LIGHTMAGENTA), 1, 1, 2);
					}
				}
			}
		}

	}

	if (fabs(A) == 0.0)//<=0.001) 
	{
		//		result[GrabNo]->lineValue[board][camera][areaNo][0] = B;
		//		result[GrabNo]->lineValue[board][camera][areaNo][1] = C;
		//		result[GrabNo]->lineValue[board][camera][areaNo][2] = (double)cnt;
		lineValue[areaNo][0] = A;
		lineValue[areaNo][1] = B;
		lineValue[areaNo][2] = C;
		lineValue[areaNo][3] = C;
		lineValue[areaNo][4] = C;
		lineValue[areaNo][5] = minY;
		lineValue[areaNo][6] = maxY;
		return C;
	}
	else
	{
		lineValue[areaNo][0] = A;
		lineValue[areaNo][1] = B;
		lineValue[areaNo][2] = C;
		lineValue[areaNo][7] = 1.0; //parabola

		if (!direction)
		{// Apex
			if (lineValue[areaNo][0] > 0.0)
			{// 왼쪽볼록
				for (i = minY; i < maxY; i++)
				{
					xApex = A*(double)i*(double)i + B*(double)i + C;
					maxXApex = maxXApex < xApex ? xApex : maxXApex;
					if (maxXApex == xApex) yApex = i;
				}
				xApex = A*yApex*yApex + B*yApex + C;
				//					result[GrabNo]->lineValue[board][camera][areaNo][3] = A*((-B)/(2.0*A))*((-B)/(2.0*A))+B*((-B)/(2.0*A))+C;//xApex;
				lineValue[areaNo][4] = xApex;
				// 					result[GrabNo]->lineValue[board][camera][areaNo][5] = (-B)/(2.0*A);//yApex;
				lineValue[areaNo][6] = yApex;
				for (i = minY; i < maxY; i++)
				{
					xApex = A*(double)i*(double)i + B*(double)i + C;
					minXApex = minXApex > xApex ? xApex : minXApex;
					if (minXApex == xApex) yApex = i;
				}
				xApex = A*yApex*yApex + B*yApex + C;
				lineValue[areaNo][3] = xApex;
				lineValue[areaNo][5] = yApex;

				return lineValue[areaNo][4];

			}
			else
			{// 오른쪽볼록
				for (i = minY; i < maxY; i++)
				{
					xApex = A*(double)i*(double)i + B*(double)i + C;
					minXApex = minXApex > xApex ? xApex : minXApex;
					if (minXApex == xApex) yApex = i;
				}
				xApex = A*yApex*yApex + B*yApex + C;
				lineValue[areaNo][3] = xApex;
				//					result[GrabNo]->lineValue[board][camera][areaNo][4] = A*((-B)/(2.0*A))*((-B)/(2.0*A))+B*((-B)/(2.0*A))+C;//xApex;
				lineValue[areaNo][5] = yApex;
				//					result[GrabNo]->lineValue[board][camera][areaNo][6] = (-B)/(2.0*A);//yApex;
				for (i = minY; i < maxY; i++)
				{
					xApex = A*(double)i*(double)i + B*(double)i + C;
					maxXApex = maxXApex < xApex ? xApex : maxXApex;
					if (maxXApex == xApex) yApex = i;
				}
				xApex = A*yApex*yApex + B*yApex + C;
				lineValue[areaNo][4] = xApex;
				lineValue[areaNo][6] = yApex;

				return lineValue[areaNo][4];

			}
		}
		else
		{
			if (lineValue[areaNo][0] < 0.0)
			{//아래로볼록
				for (i = minY; i < maxY; i++)
				{
					xApex = A*(double)i*(double)i + B*(double)i + C;
					minXApex = minXApex > xApex ? xApex : minXApex;
					if (minXApex == xApex) yApex = i;
				}
				xApex = A*yApex*yApex + B*yApex + C;
				lineValue[areaNo][3] = xApex;
				lineValue[areaNo][4] = A*((-B)/(2.0*A))*((-B)/(2.0*A))+B*((-B)/(2.0*A))+C;//xApex;
				lineValue[areaNo][5] = yApex;
				lineValue[areaNo][6] = (-B)/(2.0*A);//yApex;


				return lineValue[areaNo][3];
			}
			else
			{//위로볼록
				for (i = minY; i < maxY; i++)
				{
					xApex = A*(double)i*(double)i + B*(double)i + C;
					maxXApex = maxXApex < xApex ? xApex : maxXApex;
					if (maxXApex == xApex) yApex = i;
				}
				xApex = A*yApex*yApex + B*yApex + C;
				lineValue[areaNo][3] = A*((-B)/(2.0*A))*((-B)/(2.0*A))+B*((-B)/(2.0*A))+C;//xApex;
				lineValue[areaNo][4] = xApex;
				lineValue[areaNo][5] = (-B)/(2.0*A);//yApex;
				lineValue[areaNo][6] = yApex;


				return lineValue[areaNo][3];
			}
		}
	}

	return 0.0;
}

BOOL CInspFunc::AlignWafer(unsigned char *ImageBuffer, int imgSizeX, int imgSizeY, int NochKind, double WaferInchSize, IplImage** resImg, double *dDegree)
{
	memset(lineValue, 0x00, sizeof(lineValue));
	memset(resEdgePoint, 0x00, sizeof(resEdgePoint));
	memset(resEdgeCnt, 0x00, sizeof(resEdgeCnt));
	memset(Sigma_Point, 0x00, sizeof(Sigma_Point));
	memset(Sigma_PointCnt, 0x00, sizeof(Sigma_PointCnt));

	enum
	{
		NKIND_PLAT = 0,
		NKIND_NOCH,

		MAX_NKIND_CNT
	};

	CRect r = CRect(0,0,imgSizeX,imgSizeY);

	int nDirection = 1;
	int nArea = 0;
	int nDiffTh = pModelData->iDiffTh;
	int nDiffPixel = pModelData->iDiffPixel;
	int nVal = pModelData->iVal;
	int nValDir = pModelData->iValDir;
	int nScandir = pModelData->iScandir;
	float nSigmaSTD = 1.5;

	int nOverlayColor = 2;
	SearchEdge(1, imgSizeX, imgSizeY, 0, ImageBuffer,
		r, nArea, nDiffTh, nDiffPixel, nVal, nValDir,
		nScandir, nOverlayColor, nSigmaSTD);

	Parabola_gradientAverage_Double(imgSizeX, imgSizeY, resEdgePoint[nArea], resEdgeCnt[nArea], 1, nArea, nSigmaSTD, FALSE);

	if (resEdgeCnt[nArea] <= 10)
	{
		return FALSE;
	}

	switch (NochKind)
	{
	case NKIND_PLAT:
		if (fabs(lineValue[nArea][0]) > 0.00005)
			return FALSE;
		break;

	case NKIND_NOCH:
		CvPoint drawPt[1];
		int CompY;
		int count_sigma = 0;
		for (int i = 0; i < resEdgeCnt[nArea]; i++)
		{
			CompY = (int)(lineValue[nArea][0] * resEdgePoint[nArea][i].x*resEdgePoint[nArea][i].x + lineValue[nArea][1] * resEdgePoint[nArea][i].x + lineValue[nArea][2]);
			if (CompY + 30 < resEdgePoint[nArea][i].y)
			{
				drawPt[0].x = (int)resEdgePoint[nArea][i].x;
				drawPt[0].y = (int)resEdgePoint[nArea][i].y;
				cvLib->DoDrawOverlay(pCvDlg->ImgBuff_OverlayDisp, DRAW_TYPE_CROSS, drawPt, cvScalar(CV_YELLOW), 1, 1, 2);

				Sigma_Point[nArea][count_sigma].x = (float)resEdgePoint[nArea][i].x;
				Sigma_Point[nArea][count_sigma].y = (float)resEdgePoint[nArea][i].y;

				count_sigma++;
				Sigma_PointCnt[nArea] = count_sigma;
			}
		}
		break;
	}

	nArea = 1;
	double gradient = 0.0;
	double yValue = 0.0;
	double Apex_X, Apex_Y;

	switch(NochKind)
	{
	case NKIND_PLAT:
		LinegradientAverage_Double(imgSizeX, imgSizeY, resEdgePoint[0], resEdgeCnt[0], nDirection, nArea, nSigmaSTD, &gradient, &yValue, TRUE);
		*dDegree = (-1.0)*gradient * (180.0 / PI);
		break;

	case NKIND_NOCH:

		nSigmaSTD = (float)pModelData->dSigmaSTD;
		Parabola_gradientAverage_Double(imgSizeX, imgSizeY, Sigma_Point[0], Sigma_PointCnt[0], nDirection, nArea, nSigmaSTD, TRUE);
		Apex_X = lineValue[nArea][6];
		Apex_Y = lineValue[nArea][4];

		if (Apex_Y < lineValue[0][0] * Apex_X*Apex_X + lineValue[0][1] * Apex_X + lineValue[0][2])
			return FALSE;

		*dDegree = (0.5*imgSizeX - Apex_X)*(9.8039/1000.0)*360/(PI*WaferInchSize*DEF_INCH_RESOL);
		break;
	}

	*resImg = pCvDlg->ImgBuff_OverlayDisp;//결과 test 확인 필요

	return TRUE;
}
#endif
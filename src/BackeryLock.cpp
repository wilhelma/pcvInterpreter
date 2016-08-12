/**
 *
 *    @file  ThreadStack.h
 *   @brief  Implementation file for the BackeryLock and BakeryLocks classes. 
 *
 *    @date  12/08/16
 *  @author  Nathaniel Knapp (github.com/deknapp),
 *           <nathaniel.knapp@gmail.com>
 */


BakeryLock::BakeryLock() {
	

}

BakeryLock::lexIsLess(int i, int k) {


}

void BakeryLock::lock(TRD_ID thread_id) {

	int i = thread_id;
	waiting_flag.at(i) = TRUE;
	int max_label = *(std::max_element(order_label.begin(), C.end()));
	order_label.at(i) = max_element + 1;
	k = 0;
	while (waiting_flag.at(k) && lexIsLess(i, k)) {
		if (waiting_flag.size() < 2)
			break;
		if (k == (waiting_flag.size() - 1))
			k = 0;
		else
			k++;
	}
}

void BakeryLock::unlock(TRD_ID thread_id) {

	waiting_flag[thread_id] = FALSE;
}
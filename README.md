# stock exchange matching engine
The input comes from stdin where each line will contain several columns
The first column is the order type:
1. BUY 
2. SELL
3. CANCEL
4. MODIFY
5. PRINT

If the first column is BUY or SELL the lines will have five columns in total
1. The second column will be IOC (Insert of cancel) or GFD (Good for Day)
2. The third column indicates the price to buy/sell at, it is an integer.
3. The fourth column show the quantity to buy/sell.
4. The fifth column is the orderID

If the first column is CANCEL then the line contains two columns with second column denoting the orderID to be deleted.

If the first column is MODIFY the the line will five columns
1. The second column is OrderId to be modified.
2. The third column is buy/sell.
3. The fourth column is the new price of the order. 
4. The fifth column is the new quantity of that order.

GFD order will remain on the book until it has been traded
IOC order if cannot be traded immidiately, it will be cancelled right away. If it is patially traded non-traded part is cancelled. 

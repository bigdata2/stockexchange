# stock exchange matching engin
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

